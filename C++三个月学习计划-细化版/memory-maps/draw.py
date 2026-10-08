#!/usr/bin/env python3
"""用 PyMuPDF 画 p1_01 的内存结构图（不依赖浏览器渲染）。

输出：p1_01_memory_map.pdf / .svg / -N.png（1 格 = 1 字节）
"""
from __future__ import annotations

import sys
from pathlib import Path

import fitz

WORK = Path("/tmp/memmap")
OUT = Path(sys.argv[1]) if len(sys.argv) > 1 else WORK / "out"

PAGE_W, PAGE_H = 595.276, 841.89
MARGIN_X, MARGIN_TOP, MARGIN_BOTTOM = 34.02, 39.7, 39.7
CONTENT_W = PAGE_W - 2 * MARGIN_X

FONT = "china-ss"
STROKE = (0.45, 0.45, 0.45)
TEXT = (0.0, 0.0, 0.0)

COLORS = {
    "member": (0.81, 0.88, 0.97),
    "pad": (0.94, 0.94, 0.94),
    "vptr": (0.97, 0.84, 0.72),
    "placeholder": (0.91, 0.91, 0.96),
}

LAYOUTS = [
    ("Empty", "空类：为什么不是 0 字节", [("占位字节", 1, "placeholder")],
     "C++ 要求每个对象都有唯一地址，空类也必须占 1 字节。"),
    ("OneChar", "只含一个 char", [("char c", 1, "member")],
     "char 对齐 = 1，所以大小就是 1，没有任何填充。"),
    ("CharInt", "char 在前、int 在后", [("char c", 1, "member"), ("填充", 3, "pad"), ("int i", 4, "member")],
     "int 必须落在 4 的倍数上：char 占偏移 0，int 只能从偏移 4 开始，中间 3 字节变成填充；整体按 4 对齐。"),
    ("IntChar", "int 在前、char 在后", [("int i", 4, "member"), ("char c", 1, "member"), ("填充", 3, "pad")],
     "int 在偏移 0、char 在偏移 4；整体必须按 4 对齐，尾部又补 3 字节 —— 大小是 8，不是 5。"),
    ("HasFunction", "只有非虚成员函数", [("占位字节（无数据成员）", 1, "placeholder")],
     "成员函数不占对象内存：它编译成普通函数，调用时额外把 this 传进去，所以 sizeof 仍是 1。"),
    ("HasVirtual", "有一个虚函数", [("vptr（虚表指针）", 8, "vptr")],
     "出现虚函数后对象里多出一个 vptr（64 位下 8 字节），它指向该类唯一的虚表。"),
    ("WithVirtual", "int + 两个虚函数（例题 1-4）",
     [("int value", 4, "member"), ("填充", 4, "pad"), ("vptr", 8, "vptr")],
     "数据成员先按对齐落位（int 在 0，补到偏移 8），再放 vptr —— 16 字节。"),
    ("LooseLayout", "char → double → int（例题 1-2）",
     [("char c", 1, "member"), ("填充", 7, "pad"), ("double d", 8, "member"),
      ("int i", 4, "member"), ("填充", 4, "pad")],
     "double 要 8 对齐，char 后面跳 7 字节；int 之后整体再补到 8 的倍数 —— 24 字节。"),
    ("TightLayout", "double → int → char（例题 1-2）",
     [("double d", 8, "member"), ("int i", 4, "member"), ("char c", 1, "member"), ("填充", 3, "pad")],
     "同样的三个成员，只改了顺序：16 字节，比上面省 8 字节 —— 成员排序也是优化。"),
]

RULES = [
    "成员按自身对齐要求落位：char=1、int=4、double=8、指针=8；放不下就往后挪，空洞变成填充。",
    "结构体整体按最大成员对齐：尾部常要补一段，保证数组里每个元素都对齐（IntChar 是 8 的原因）。",
    "空类占 1 字节占位；有虚函数多加 8 字节 vptr —— 这两条与「有几个成员」无关。",
]


def text_width(text: str, size: float, font: str = FONT) -> float:
    return fitz.get_text_length(text, fontname=font, fontsize=size)


def wrap(text: str, size: float, width: float, font: str = FONT) -> list[str]:
    lines, current = [], ""
    for char in text:
        trial = current + char
        if text_width(trial, size, font) > width and current:
            lines.append(current)
            current = char
        else:
            current = trial
    if current:
        lines.append(current)
    return lines


class Painter:
    def __init__(self) -> None:
        self.doc = fitz.open()
        self.page = None
        self.y = 0.0
        self.new_page()

    def new_page(self) -> None:
        self.page = self.doc.new_page(width=PAGE_W, height=PAGE_H)
        self.y = MARGIN_TOP

    def ensure(self, height: float) -> None:
        if self.y + height > PAGE_H - MARGIN_BOTTOM:
            self.new_page()

    def draw_text(self, x: float, y: float, text: str, size: float = 9.0,
                  color=TEXT) -> None:
        self.page.insert_text((x, y), text, fontname=FONT, fontsize=size, color=color)


def draw_card(p: Painter, name: str, subtitle: str, spans: list, note: str) -> None:
    total = sum(width for _label, width, _kind in spans)
    cell_w = min(26.0, CONTENT_W / total)
    cell_h = 20.0
    p.ensure(96)

    p.draw_text(MARGIN_X, p.y + 9, name, size=10.5)
    name_w = text_width(name, 10.5)
    p.draw_text(MARGIN_X + name_w + 6, p.y + 9, subtitle, size=7.6, color=(0.35, 0.35, 0.35))
    size_label = "sizeof = {} 字节".format(total)
    p.draw_text(PAGE_W - MARGIN_X - text_width(size_label, 8.6), p.y + 9, size_label, size=8.6)
    p.y += 14

    x = MARGIN_X
    top = p.y
    for label, width, kind in spans:
        rect = fitz.Rect(x, top, x + cell_w * width, top + cell_h)
        p.page.draw_rect(rect, color=STROKE, fill=COLORS[kind], width=0.5)
        if kind == "pad":
            offset = 4.0
            while offset < cell_h:
                p.page.draw_line(fitz.Point(rect.x0, rect.y0 + offset),
                                 fitz.Point(rect.x1, rect.y0 + offset),
                                 color=(0.86, 0.86, 0.86), width=0.4)
                offset += 4.0
        if cell_w * width >= 20:
            label_size = 6.6 if cell_w * width >= 34 else 5.8
            while text_width(label, label_size) > (cell_w * width - 3) and label_size > 4.6:
                label_size -= 0.2
            p.draw_text(x + (cell_w * width - text_width(label, label_size)) / 2,
                        top + cell_h / 2 + label_size * 0.36, label, size=label_size)
        x += cell_w * width
    p.y = top + cell_h

    x = MARGIN_X
    for _label, width, _kind in spans:
        if cell_w * width >= 20:
            p.draw_text(x + 0.8, p.y + 6, str(int((x - MARGIN_X) / cell_w)),
                        size=5.8, color=(0.4, 0.4, 0.4))
            p.page.draw_line(fitz.Point(x, p.y), fitz.Point(x, p.y + 2.5),
                             color=(0.6, 0.6, 0.6), width=0.4)
        x += cell_w * width
    p.y += 9

    for line in wrap(note, 7.6, CONTENT_W):
        p.draw_text(MARGIN_X, p.y + 7, line, size=7.6, color=(0.15, 0.15, 0.15))
        p.y += 9.6
    p.y += 5.5


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    p = Painter()

    p.draw_text(MARGIN_X, p.y + 12, "p1_01_sizeof.cpp · 内存结构图", size=15)
    p.y += 20
    header = "按本机实测绘制（Apple clang 21 / arm64 / macOS）：1 格 = 1 字节，数据成员、填充、vptr 分色。"
    for line in wrap(header, 8, CONTENT_W):
        p.draw_text(MARGIN_X, p.y + 7, line, size=8, color=(0.35, 0.35, 0.35))
        p.y += 10
    p.y += 4

    legend = [("member", "数据成员"), ("pad", "对齐填充"),
              ("vptr", "vptr（虚表指针）"), ("placeholder", "占位字节")]
    x = MARGIN_X
    for kind, label in legend:
        p.page.draw_rect(fitz.Rect(x, p.y, x + 11, p.y + 8), color=STROKE,
                         fill=COLORS[kind], width=0.5)
        p.draw_text(x + 14, p.y + 6.6, label, size=7.4)
        x += 14 + text_width(label, 7.4) + 14
    p.y += 16

    for name, subtitle, spans, note in LAYOUTS:
        draw_card(p, name, subtitle, spans, note)

    p.ensure(80)
    box_top = p.y
    height = 20.0
    for rule in RULES:
        height += 10.5 * len(wrap("{}. {}".format(RULES.index(rule) + 1, rule), 8, CONTENT_W - 14)) + 1.5
    p.page.draw_rect(fitz.Rect(MARGIN_X, box_top, PAGE_W - MARGIN_X, box_top + height),
                     color=(0, 0, 0), width=0.8)
    p.y = box_top + 13
    p.draw_text(MARGIN_X + 6, p.y, "三条规则", size=10)
    p.y += 6
    for index, rule in enumerate(RULES, start=1):
        for line in wrap("{}. {}".format(index, rule), 8, CONTENT_W - 14):
            p.draw_text(MARGIN_X + 6, p.y + 7, line, size=8)
            p.y += 10.5
        p.y += 1.5

    pdf_path = OUT / "p1_01_memory_map.pdf"
    p.doc.save(pdf_path)
    (OUT / "p1_01_memory_map.svg").write_text(
        "\n".join(pg.get_svg_image(text_as_path=True) for pg in p.doc), encoding="utf-8"
    )
    for index in range(p.doc.page_count):
        pix = p.doc[index].get_pixmap(matrix=fitz.Matrix(150 / 72, 150 / 72))
        pix.save(OUT / "p1_01_memory_map-{}.png".format(index + 1))

    print("pages:", p.doc.page_count)
    print("pdf:", pdf_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
