#!/usr/bin/env python3
"""重建「C++ 底层例题代码集」讲义的 HTML / PDF / 预览图。

在原来的排版（print-sheet 技能的 scripts/sheet.py）之上做两件事：

1. 把 memory-maps/ 里的内存结构图插到对应例题的代码页之后，一节 = 一页；
   图片内嵌成 base64 —— macOS 的 WebKit 渲染器只给 HTML 所在目录的读取权限，
   引用 ../memory-maps/xxx.png 会加载失败。
2. 顺手把 05-例题代码索引.md 的「页」列更新成实际页码（插图会让后面整体后移）。

用法：
    python3 print-out-examples/build_handout.py               # 完整重建
    python3 print-out-examples/build_handout.py --no-preview  # 不生成预览图
    python3 print-out-examples/build_handout.py --skip-index-fix

依赖：本机 python3 能 import fitz（PyMuPDF）；渲染走 macOS 自带的 WebKit。
"""
from __future__ import annotations

import argparse
import base64
import os
import re
import struct
import sys
from pathlib import Path
from types import SimpleNamespace

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
CODE = ROOT / "code"
MAPS = ROOT / "memory-maps"
INDEX_MD = ROOT / "05-例题代码索引.md"

NAME = "cpp_examples_handout"
TITLE = "C++ 底层例题代码集（17 例）"

# 插在哪个例题的代码页之后 → [(图片路径, 图注)]
FIGURES = {
    "p1_01_sizeof.cpp": [
        (MAPS / "p1_01_memory_map-1.png",
         "p1_01_sizeof.cpp · 内存结构图（1 格 = 1 字节，按本机实测绘制）"),
    ],
    "p1_02_layout.cpp": [
        (MAPS / "p1_02_memory_map-1.png",
         "p1_02_layout.cpp · 内存结构图 第 1 页 / 共 2 页（成员顺序 + 偏移标尺 + 原始字节）"),
        (MAPS / "p1_02_memory_map-2.png",
         "p1_02_layout.cpp · 内存结构图 第 2 页 / 共 2 页（WithString 与 std::string 的两种形态）"),
    ],
}

PAPERS = {"A4": (595.276, 841.89), "A5": (419.53, 595.28), "A3": (841.89, 1190.55),
          "Letter": (612.0, 792.0), "Legal": (612.0, 1008.0)}

FIGURE_CSS = """
/* 内存结构图页：一节 = 一页，图片按可用高度缩放，保证不会被切页 */
.figure-sheet { break-inside: avoid; page-break-inside: avoid; }
.figure-sheet figure { margin: 0; }
.figure-sheet img {
  display: block;
  margin: 0 auto;
  max-width: 100%;
  height: auto;
  border: .5pt solid #bbb;
  background: #fff;
}
.figure-sheet figcaption {
  margin-top: 1.5mm;
  font-size: ${code_pt}pt;
  color: #333;
  text-align: center;
}
"""

# 图高安全系数：图片按百分比铺到容器宽度，容器宽度又受滚动条影响（实测 527px 视口里
# 可用 510px），所以按整页宽算出来的百分比要再留一点余量，避免"图片 + 图注"超高一页、
# 把图注挤到下一页。
FIGURE_HEIGHT_SAFETY = 1.04

# WebKit 的打印路径里 1 CSS px = 1 PDF pt（实测：527px 视口 → 527pt 纸宽），
# 所以下面所有长度都按"px/pt 同值"来算。


def load_sheet_module():
    """把 print-sheet 技能的 sheet.py 当模块加载，复用它的排版与渲染逻辑。"""
    candidates = [
        os.environ.get("PRINT_SHEET_SCRIPTS"),
        str(Path.home() / ".codex/skills/print-sheet/scripts"),
        str(Path.home() / ".agents/skills/print-sheet/scripts"),
    ]
    for item in candidates:
        if not item:
            continue
        path = Path(item)
        if (path / "sheet.py").is_file():
            sys.path.insert(0, str(path))
            import sheet  # type: ignore

            return sheet
    raise SystemExit("找不到 print-sheet 技能的 sheet.py（可用 PRINT_SHEET_SCRIPTS 指定目录）")


def png_size(path: Path) -> tuple[int, int]:
    """只读 PNG 头拿宽高，避免多依赖一个图像库。"""
    with path.open("rb") as fh:
        head = fh.read(26)
    if head[:8] == b"\x89PNG\r\n\x1a\n" and head[12:16] == b"IHDR":
        return struct.unpack(">II", head[16:24])
    raise SystemExit("不是 PNG：{}".format(path))


def data_uri(path: Path) -> str:
    mime = "image/png" if path.suffix.lower() == ".png" else "image/jpeg"
    return "data:{};base64,{}".format(mime, base64.b64encode(path.read_bytes()).decode("ascii"))


def figure_section(source: str, image: Path, caption: str, width: str) -> str:
    return (
        '<section class="sheet figure-sheet">'
        '<div class="file-head"><span class="name">{source} · 内存结构图</span>'
        '<span class="sub">memory-maps/</span></div>'
        '<figure><img src="{uri}" alt="{alt}" style="width:{width}">'
        '<figcaption>{caption}</figcaption></figure></section>'
    ).format(
        source=source,
        uri=data_uri(image),
        alt=caption,
        width=width,
        caption=caption,
    )


def available_box(sheet, opts) -> tuple[float, float]:
    """图页可用区（pt）：整页去掉页边距、文件头、图注和一点余量。

    这里的预留值是按实测校准的 —— 预留太小会让图注被挤到下一页
    （一节就变成两页）。字号变了按比例跟着变。
    """
    paper_w, paper_h = PAPERS[opts.paper]
    if opts.landscape:
        paper_w, paper_h = paper_h, paper_w
    top, right, bottom, left = sheet.parse_margin(opts.margin)
    box_w = paper_w - left - right
    box_h = paper_h - top - bottom
    head = opts.font_size * 1.5 + 25.0     # .file-head：标题行 + 内边距 + 下边距
    caption = opts.font_size * 1.5 + 8.0   # figcaption：一行 + 上边距
    head += 4.0                            # 余量：字体回退会让行高比 1.5 略高
    return box_w, max(box_h - head - caption, 100.0)


def inject_figures(sheet, html: str, labels: list[str], opts, width_pt: float,
                   box_h: float) -> tuple[str, list[int]]:
    """把图页插到对应文件那一节之后，返回 (新 HTML, 插入位置列表)。"""
    parts = html.split("</section>")
    if len(parts) < len(labels) + 1:
        raise SystemExit("HTML 分节数和输入文件数对不上（{} vs {}）".format(len(parts) - 1, len(labels)))
    inserted: list[int] = []
    for index, label in enumerate(labels):
        figures = FIGURES.get(label)
        if not figures:
            continue
        blocks = []
        for image, caption in figures:
            if not image.is_file():
                raise SystemExit("缺少图片：{}（先跑 memory-maps/draw*.py 生成）".format(image))
            pixels_w, pixels_h = png_size(image)
            # 按「整页宽」估出能占的百分比：图高 = 100%宽 ÷ 宽高比，要在可用高度内
            percent = (box_h / FIGURE_HEIGHT_SAFETY) * pixels_w / pixels_h / width_pt * 100.0
            percent = max(30.0, min(100.0, percent))
            blocks.append(figure_section(label, image, caption, "{:.1f}%".format(percent)))
        parts[index + 1] = "\n" + "\n".join(blocks) + parts[index + 1]
        inserted.append(index)
    return "</section>".join(parts), inserted


def make_opts(args) -> SimpleNamespace:
    return SimpleNamespace(
        title=TITLE,
        name=NAME,
        paper=args.paper,
        landscape=False,
        margin=args.margin,
        font=args.font,
        font_size=args.font_size,
        no_line_numbers=False,
        no_comment_style=False,
        md_renderer="auto",
        renderer=args.renderer,
        timeout=args.timeout,
        dpi=args.dpi,
        preview_pages=args.preview_pages,
        no_preview=args.no_preview,
    )


def collect_sources(sheet) -> list[tuple[str, Path, str, bool]]:
    files = [INDEX_MD] + sorted(CODE.glob("p*.cpp"))
    sources = []
    for path in files:
        text = sheet.read_text(path)
        if text is None:
            raise SystemExit("读不了文本文件：{}".format(path))
        sources.append((path.name, path, text, path.suffix.lower() in sheet.MARKDOWN_EXT))
    return sources


def page_map(pdf_path: Path, labels: list[str]) -> dict[str, int]:
    """扫 PDF，找出每个文件那一节起始的实际页码。"""
    import fitz

    doc = fitz.open(pdf_path)
    found: dict[str, int] = {}
    for number, page in enumerate(doc, start=1):
        text = page.get_text().lstrip()
        for label in labels:
            if label in found:
                continue
            # 代码页的文件头是「<文件名>\n<行数> 行」，图页的文件头后面跟
            # "memory-maps/"，不能被误认成代码页。
            # 注意：从 WebKit 导出的 PDF 里取文本时，"行" 会变成兼容字形 "⾏"，
            # 所以这里只匹配「文件名 + 换行 + 数字」，不匹配那个汉字。
            if re.match(re.escape(label) + r"\n\d+ ", text):
                found[label] = number
    return found


def sync_index(sheet, mapping: dict[str, int]) -> bool:
    """把索引 md 里每个例题的页码改成实际页码，返回是否有改动。"""
    text = INDEX_MD.read_text(encoding="utf-8")
    updated = text
    for label, page in mapping.items():
        pattern = re.compile(r"^(\|\s*)\d+(\s*\|\s*`" + re.escape(label) + r"`)", re.M)
        updated = pattern.sub(lambda m: "{}{}{}".format(m.group(1), page, m.group(2)), updated)
    if updated == text:
        return False
    INDEX_MD.write_text(updated, encoding="utf-8")
    return True


def build(sheet, opts, sources, out_dir: Path, write: bool = True) -> tuple[Path, Path]:
    html = sheet.build_html(sources, opts)
    box_w, box_h = available_box(sheet, opts)
    labels = [label for label, _p, _t, _md in sources]
    html, _ = inject_figures(sheet, html, labels, opts, box_w, box_h)
    html = html.replace("</style>", FIGURE_CSS.replace("${code_pt}", "{:.2f}".format(opts.font_size)) + "</style>")
    html_path = out_dir / (opts.name + ".html")
    pdf_path = out_dir / (opts.name + ".pdf")
    if write:
        html_path.write_text(html, encoding="utf-8")
        used = sheet.render_pdf(html_path, pdf_path, opts.renderer, opts.timeout,
                                paper=opts.paper, landscape=opts.landscape, margin=opts.margin)
        print("PDF 渲染器：{}".format(used))
    return html_path, pdf_path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("-o", "--out", default=str(HERE), help="输出目录")
    parser.add_argument("--paper", default="A4")
    parser.add_argument("--margin", default="14mm 12mm")
    parser.add_argument("--font", default=None, help="字体栈（默认用技能里的 macOS/Linux 默认值）")
    parser.add_argument("--font-size", type=float, default=7.4)
    parser.add_argument("--renderer", default="auto", choices=["auto", "chromium", "webkit", "soffice"])
    parser.add_argument("--timeout", type=int, default=180)
    parser.add_argument("--dpi", type=int, default=100)
    parser.add_argument("--preview-pages", type=int, default=6)
    parser.add_argument("--no-preview", action="store_true")
    parser.add_argument("--skip-index-fix", action="store_true",
                        help="不改 05-例题代码索引.md 的页码（默认会同步）")
    args = parser.parse_args()

    sheet = load_sheet_module()
    if args.font is None:
        args.font = sheet.MAC_FONT if sys.platform == "darwin" else sheet.DEFAULT_FONT
    opts = make_opts(args)
    out_dir = Path(args.out).expanduser().resolve()
    out_dir.mkdir(parents=True, exist_ok=True)
    sources = collect_sources(sheet)
    print("输入：{} 个文件（含 {} 张插图页）".format(
        len(sources), sum(len(v) for v in FIGURES.values())))

    html_path, pdf_path = build(sheet, opts, sources, out_dir)

    if not args.skip_index_fix:
        labels = [label for label, _p, _t, _md in sources]
        mapping = page_map(pdf_path, labels)
        if sync_index(sheet, mapping):
            print("已同步 05-例题代码索引.md 的页码，重新排版一次")
            sources = collect_sources(sheet)
            html_path, pdf_path = build(sheet, opts, sources, out_dir)

    info = sheet.pdf_info(pdf_path)
    print("HTML: {}".format(html_path))
    print("PDF : {} （{} 页，{}）".format(pdf_path, info.get("pages", "?"), info.get("page size", "?")))
    if not args.no_preview:
        previews = sheet.make_preview(pdf_path, out_dir / opts.name, args.dpi, args.preview_pages)
        print("预览: {}".format(" ".join(str(p) for p in previews) or "（生成失败）"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
