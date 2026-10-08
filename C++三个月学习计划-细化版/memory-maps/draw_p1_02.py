#!/usr/bin/env python3
"""用 PyMuPDF 画 p1_02_layout.cpp 的内存结构图（风格对齐 p1_01 那张）。

输出：p1_02_memory_map.pdf / .svg / -N.png（1 格 = 1 字节）

图上的每个数字都来自本机实测（Apple clang 21 / arm64 / macOS）：

    clang++ -std=c++17 -O0 code/p1_02_layout.cpp -o /tmp/p102 && /tmp/p102
    clang++ -std=c++17 -O0 /tmp/probe_final.cpp        # std::string 短/长串对照
    clang++ -std=c++17 -O0 /tmp/probe_pad.cpp          # 0xAB + placement new 验证填充

用法：python3 memory-maps/draw_p1_02.py [输出目录]
"""
from __future__ import annotations

import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

import fitz  # noqa: E402

import draw as base  # 复用页面尺寸 / 字体 / Painter / 配色  # noqa: E402

OUT = Path(sys.argv[1]) if len(sys.argv) > 1 else HERE

PAGE_W, PAGE_H = base.PAGE_W, base.PAGE_H
MARGIN_X, MARGIN_TOP, MARGIN_BOTTOM = base.MARGIN_X, base.MARGIN_TOP, base.MARGIN_BOTTOM
CONTENT_W = base.CONTENT_W
FONT = base.FONT
MONO = "cour"
STROKE = base.STROKE
TEXT = base.TEXT
HINT = (0.35, 0.35, 0.35)
DIM = (0.45, 0.45, 0.45)

COLORS = dict(base.COLORS)
COLORS.update({
    "lib": (0.82, 0.94, 0.86),     # 库类型（std::string）内部
    "undef": (0.99, 0.91, 0.91),   # 填充：内容未定义
    "byte": (0.985, 0.985, 0.985),
    "heap": (0.90, 0.90, 0.96),
})


def fit_size(text: str, max_w: float, start: float = 6.6, minimum: float = 4.4):
    """把标签缩到能塞进 max_w；缩到 minimum 还塞不下就返回 None。"""
    size = start
    while size > minimum and base.text_width(text, size) > max_w:
        size -= 0.2
    return size if base.text_width(text, size) <= max_w else None


def draw_mono(p: "base.Painter", x: float, y: float, text: str,
              size: float = 5.2, color=TEXT) -> None:
    p.page.insert_text((x, y), text, fontname=MONO, fontsize=size, color=color)


def draw_field_row(p, spans, cell_w, row_h: float = 20.0) -> None:
    """spans: (标签, 字节数, 配色[, 短标签])。

    1 字节的格子放不下 "char c" 这种长标签，就用短标签（如 "c"），
    这样每个字段都有名字可读，也不会压到隔壁格子。
    """
    top = p.y
    x = MARGIN_X
    for span in spans:
        label, width, kind = span[0], span[1], span[2]
        short = span[3] if len(span) > 3 else label
        w = cell_w * width
        rect = fitz.Rect(x, top, x + w, top + row_h)
        p.page.draw_rect(rect, color=STROKE, fill=COLORS[kind], width=0.5)
        if kind in ("pad", "undef"):
            offset = 4.0
            while offset < row_h:
                p.page.draw_line(fitz.Point(rect.x0, rect.y0 + offset),
                                 fitz.Point(rect.x1, rect.y0 + offset),
                                 color=(0.86, 0.86, 0.86), width=0.4)
                offset += 4.0
        size = fit_size(label, w - 3.0)
        if size is None:
            label, size = short, fit_size(short, w - 3.0)
        if size is None:
            label, size = short, 4.4
        if size:
            p.draw_text(x + (w - base.text_width(label, size)) / 2,
                        top + row_h / 2 + size * 0.36, label, size=size)
        x += w
    p.y = top + row_h


def draw_ruler(p, total: int, cell_w: float, step: int = 4) -> None:
    top = p.y
    for i in range(0, total, step):
        x = MARGIN_X + cell_w * i
        p.page.draw_line(fitz.Point(x, top), fitz.Point(x, top + 2.5),
                         color=(0.6, 0.6, 0.6), width=0.4)
        p.draw_text(x + 0.8, top + 6.2, str(i), size=5.4, color=DIM)
    p.y += 9


def draw_byte_row(p, values: list, cell_w: float, cell_h: float = 11.0) -> None:
    for i, value in enumerate(values):
        x = MARGIN_X + cell_w * i
        rect = fitz.Rect(x, p.y, x + cell_w, p.y + cell_h)
        p.page.draw_rect(rect, color=(0.78, 0.78, 0.78), fill=COLORS["byte"], width=0.35)
        text = value if value else "??"
        size = 5.2
        while size > 4.0 and base.text_width(text, size, MONO) > cell_w - 1.0:
            size -= 0.2
        draw_mono(p, x + (cell_w - base.text_width(text, size, MONO)) / 2,
                  p.y + cell_h / 2 + size * 0.36, text, size=size,
                  color=(0.25, 0.25, 0.25) if value else (0.72, 0.18, 0.18))
    p.y += cell_h + 3
    p.draw_text(MARGIN_X, p.y + 4.5,
                "下排 = 本例 dump 出的原始字节（小端；?? = 填充内容未定义，实测每次运行都不同）",
                size=6.8, color=HINT)
    p.y += 7.5


def draw_notes(p, notes: list) -> None:
    p.y += 2
    for note in notes:
        first = True
        for line in base.wrap(note, 7.6, CONTENT_W - 7):
            if first:
                p.draw_text(MARGIN_X, p.y + 7, "·", size=7.6, color=(0.3, 0.3, 0.3))
            p.draw_text(MARGIN_X + 7, p.y + 7, line, size=7.6, color=(0.15, 0.15, 0.15))
            p.y += 9.6
            first = False
        p.y += 1.0
    p.y += 5.0


def card(p, name: str, subtitle: str, spans: list, notes: list,
         bytes_row: list | None = None, ruler_step: int = 4) -> None:
    total = sum(span[1] for span in spans)
    cell_w = min(26.0, CONTENT_W / total)
    p.ensure(40 + (27 + 7.5 if bytes_row else 0) + 10.0 * len(notes))

    size_label = "sizeof = {} 字节".format(total)
    name_w = base.text_width(name, 10.5)
    avail = CONTENT_W - name_w - 6 - base.text_width(size_label, 8.6) - 10
    sub_size = 7.6
    while sub_size > 6.0 and base.text_width(subtitle, sub_size) > avail:
        sub_size -= 0.2
    if base.text_width(subtitle, sub_size) > avail:  # 还是放不下就截断
        cut = ""
        for ch in subtitle:
            if base.text_width(cut + ch + "…", sub_size) > avail:
                break
            cut += ch
        subtitle = (cut + "…") if cut else ""

    p.draw_text(MARGIN_X, p.y + 9.5, name, size=10.5)
    p.draw_text(MARGIN_X + name_w + 6, p.y + 9.5, subtitle, size=sub_size, color=HINT)
    p.draw_text(PAGE_W - MARGIN_X - base.text_width(size_label, 8.6), p.y + 9.5,
                size_label, size=8.6)
    p.y += 14

    draw_field_row(p, spans, cell_w)
    draw_ruler(p, total, cell_w, ruler_step)
    if bytes_row:
        draw_byte_row(p, bytes_row, cell_w)
    else:
        p.y += 2
    draw_notes(p, notes)


def compare_card(p, items: list, notes: list) -> None:
    """items: [(名称, 字节数, 配色)] —— 只改顺序带来的体积差。"""
    p.ensure(34 + 12 * len(items) + 10.0 * len(notes))
    p.draw_text(MARGIN_X, p.y + 9, "同样的三个成员，只改声明顺序", size=10.5)
    p.y += 14
    unit = min(8.5, 190.0 / max(total for _n, total, _k in items))
    for name, total, kind in items:
        p.draw_text(MARGIN_X, p.y + 7.5, name, size=7.6)
        size_x = MARGIN_X + 6 + base.text_width(name, 7.6) + 6
        size_text = "{} 字节".format(total)
        p.draw_text(size_x, p.y + 7.5, size_text, size=7.6)
        bar_x = size_x + base.text_width(size_text, 7.6) + 10
        rect = fitz.Rect(bar_x, p.y, bar_x + unit * total, p.y + 9)
        p.page.draw_rect(rect, color=STROKE, fill=COLORS[kind], width=0.5)
        for i in range(1, total):
            p.page.draw_line(fitz.Point(rect.x0 + unit * i, rect.y0),
                             fitz.Point(rect.x0 + unit * i, rect.y1),
                             color=(1, 1, 1), width=0.35)
        p.y += 12
    draw_notes(p, notes)


def box(p, title: str, items: list) -> None:
    wrapped = [base.wrap("{}. {}".format(i, item), 8, CONTENT_W - 20)
               for i, item in enumerate(items, start=1)]
    height = 20.0 + sum(10.5 * len(lines) + 1.5 for lines in wrapped)
    p.ensure(height + 6)
    top = p.y
    p.page.draw_rect(fitz.Rect(MARGIN_X, top, PAGE_W - MARGIN_X, top + height),
                     color=(0, 0, 0), width=0.8)
    p.draw_text(MARGIN_X + 6, top + 13, title, size=10)
    p.y = top + 19
    for index, lines in enumerate(wrapped, start=1):
        for line in lines:
            p.draw_text(MARGIN_X + 6, p.y + 7, line, size=8)
            p.y += 10.5
        p.y += 1.5
    p.y += 6


def legend(p, entries: list) -> None:
    x = MARGIN_X
    for kind, label in entries:
        p.page.draw_rect(fitz.Rect(x, p.y, x + 11, p.y + 8), color=STROKE,
                         fill=COLORS[kind], width=0.5)
        if kind in ("pad", "undef"):
            for offset in (2.5, 5.0):
                p.page.draw_line(fitz.Point(x, p.y + offset), fitz.Point(x + 11, p.y + offset),
                                 color=(0.86, 0.86, 0.86), width=0.4)
        p.draw_text(x + 14, p.y + 6.6, label, size=7.4)
        x += 14 + base.text_width(label, 7.4) + 14
    p.y += 16


# ---------------------------------------------------------------- 实测数据

def bytes_of(spec: str) -> list:
    return spec.split()


LOOSE_BYTES = bytes_of("aa 00 00 00 00 00 00 00 00 00 00 00 00 00 f0 3f "
                       "44 33 22 11 00 00 00 00")

TIGHT_BYTES = bytes_of("00 00 00 00 00 00 f0 3f 44 33 22 11 aa 00 00 00")

# WithString：偏移 4..7 与 33..39 是填充，实测每次运行都不同 → 记 ??
WITH_STRING_BYTES = (bytes_of("01 00 00 00") + [None] * 4
                     + bytes_of("61 62 63 00") + ["00"] * 19 + bytes_of("03 78")
                     + [None] * 7)

# std::string 本体（libc++，数据在前）：短串 ≤22 字符就地存
STRING_SHORT_BYTES = bytes_of("61 62 63 00") + ["00"] * 19 + bytes_of("03")

# 长串：指针 / 大小 / 容量|标志；指针值每次运行都不同 → 记 ??
STRING_LONG_BYTES = ([None] * 8 + bytes_of("28 00 00 00 00 00 00 00")
                     + bytes_of("30 00 00 00 00 00 00 80"))


def page_one(p) -> None:
    p.draw_text(MARGIN_X, p.y + 12, "p1_02_layout.cpp · 内存结构图", size=15)
    p.y += 20
    header = ("按本机实测绘制（Apple clang 21 / arm64 / macOS）：1 格 = 1 字节。"
              "上排是字段归属，中间是偏移标尺，下排是实跑 dump 出的原始字节。")
    for line in base.wrap(header, 8, CONTENT_W):
        p.draw_text(MARGIN_X, p.y + 7, line, size=8, color=HINT)
        p.y += 10
    p.y += 4

    legend(p, [("member", "数据成员"), ("undef", "填充（内容未定义）"),
               ("lib", "库类型内部"), ("vptr", "标志位 / 指针字段")])

    card(p, "LooseLayout", "char → double → int（顺序不好）",
         [("char c", 1, "member", "c"), ("填充", 7, "undef"), ("double d", 8, "member"),
          ("int i", 4, "member"), ("填充", 4, "undef")],
         ["double 要求 8 对齐：char 占偏移 0，double 只能从偏移 8 开始，"
          "中间 7 字节变成填充；int 落在偏移 16，尾部再补 4 → 24。",
          "字节全对得上：aa 是 char；f0 3f 是 double 1.0 的高位（小端）；"
          "44 33 22 11 是 int 0x11223344。",
          "这里的填充显示为 00，是因为 LooseLayout 是平凡类型，`l{}` 值初始化会连填充位一起清零；"
          "换成含非平凡成员的 WithString，{} 就不再清零填充位 —— 下一页就是反例。"],
         bytes_row=LOOSE_BYTES, ruler_step=4)

    card(p, "TightLayout", "double → int → char（调整后）",
         [("double d", 8, "member"), ("int i", 4, "member"), ("char c", 1, "member", "c"),
          ("填充", 3, "undef")],
         ["同样三个成员：8 + 4 + 1 + 3 = 16；填充被挤到尾部，只剩 3 字节。",
          "规则：成员按自身对齐落位，结构体整体按最大成员对齐 —— 把大的放前面、小的挤到最后，"
          "就能把填充集中到尾部一块。"],
         bytes_row=TIGHT_BYTES, ruler_step=4)

    compare_card(p, [("LooseLayout（char 打头）", 24, "undef"),
                     ("TightLayout（double 打头）", 16, "lib")],
                 ["同样三个成员、同样的值，只改声明顺序：24 → 16，省 8 字节（约 33%）。",
                  "Rust 默认会重排字段来省内存，C++ 不会：手动排序是 C++ 程序员的基本功。"])

    box(p, "三条规则", [
        "成员按自身对齐要求落位：char=1、int=4、double=8、指针=8；放不下就往后挪，空洞变成填充。",
        "结构体整体按最大成员对齐，尾部常要补一段，保证数组里每个元素都对齐（LooseLayout 是 8 对齐、TightLayout 也是 8 对齐）。",
        "填充属于对象的一部分（sizeof 算进去），但它的内容未定义：不要 memcmp 比较含填充的结构体，"
        "也不要按字节序列化后跨平台传 —— 这正是宿主层与前端交换数据时要先定协议的原因。",
    ])


def page_two(p) -> None:
    p.draw_text(MARGIN_X, p.y + 12, "p1_02_layout.cpp · 内存结构图（续）", size=15)
    p.y += 20
    header = ("WithString 把「库类型」也放进来：std::string 的本体大小是固定的，"
              "但字符串数据在哪，取决于长度。")
    for line in base.wrap(header, 8, CONTENT_W):
        p.draw_text(MARGIN_X, p.y + 7, line, size=8, color=HINT)
        p.y += 10
    p.y += 4

    legend(p, [("member", "数据成员"), ("undef", "填充（内容未定义）"),
               ("lib", "std::string 本体"), ("vptr", "指针 / 标志字段")])

    card(p, "WithString", "int + std::string + char",
         [("int i", 4, "member"), ("填充", 4, "undef"),
          ("std::string 本体（24 字节）", 24, "lib"), ("c", 1, "member"),
          ("填充", 7, "undef")],
         ["4 + 4 + 24 + 1 + 7 = 40。std::string 要 8 对齐，所以 i 之后空出 4 字节；"
          "c 之后为对齐到 8 的倍数再补 7 字节。",
          "算出来的是「本体 24 字节」，不是「字符串 3 字节」—— sizeof 是类型合同，"
          "数组步长、跨语言传参的 ABI 都按它走。",
          "偏移 4..7 和 33..39 是本例里唯一没被写过的位置：多次运行值都不一样，"
          "所以图上记 ??"],
         bytes_row=WITH_STRING_BYTES, ruler_step=4)

    card(p, "std::string 本体 · 短串（SSO）", "len ≤ 22：数据就放在对象里",
         [("就地缓冲 __data_：\"abc\\0\" + 未用字节", 23, "lib"),
          ("长度 = 3", 1, "member", "= 3")],
         ["实测：字符串本体 24 字节 = 23 字节缓冲 + 1 字节长度；"
          "缓冲起始地址就等于对象自身地址，没有任何堆分配。",
          "libc++ 采用「数据在前」布局：最后 1 字节是长度，len=22 时它等于 0x16；"
          "超过 22 字符（len=23）就整体切换成长串模式。"],
         bytes_row=STRING_SHORT_BYTES, ruler_step=4)

    card(p, "std::string 本体 · 长串（堆）", "len ≥ 23：对象里只留指针",
         [("数据指针 __data_ → 堆", 8, "vptr"), ("大小 = 40", 8, "member"),
          ("容量 | 长串标志（最高位为 1）", 8, "vptr")],
         ["实测 len=40：size=40（0x28）、capacity=47、data 在堆上（指针值每次运行都不同，"
          "图上记 ??）；容量字段去掉最高位是 0x30 = 48 = 容量 47 + 结尾 '\\0'。",
          "所以「std::string 占 24 字节」这句话在两种模式下都成立，"
          "但「字符串数据在哪」完全不同 —— 这就是例题 2-1 的伏笔。"],
         bytes_row=STRING_LONG_BYTES, ruler_step=4)

    box(p, "读这张图要记住的三条", [
        "填充字节的内容是未定义的。本机实测：WithString 的填充每次运行都不一样；"
        "把缓冲区先填成 0xAB、再 placement new 构造对象，构造后填充位置仍是 ab —— "
        "证明编译器根本不会去写它。",
        "std::string 本体固定 24 字节，数据的位置随长度变：≤22 字符就地存放（SSO，零堆分配），"
        "≥23 字符变成「指针 + 大小 + 容量」三个 8 字节字段。",
        "WithString 的 40 = 4(int) + 4(填充) + 24(string 本体) + 1(char) + 7(尾部填充)，"
        "与例题的 dump 逐字节对得上：字段值能认出来，填充位认不出来。",
    ])


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    p = base.Painter()

    page_one(p)
    p.new_page()
    page_two(p)

    pdf_path = OUT / "p1_02_memory_map.pdf"
    p.doc.save(pdf_path)
    (OUT / "p1_02_memory_map.svg").write_text(
        "\n".join(pg.get_svg_image(text_as_path=True) for pg in p.doc), encoding="utf-8"
    )
    for index in range(p.doc.page_count):
        pix = p.doc[index].get_pixmap(matrix=fitz.Matrix(150 / 72, 150 / 72))
        pix.save(OUT / "p1_02_memory_map-{}.png".format(index + 1))

    print("pages:", p.doc.page_count)
    print("pdf:", pdf_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
