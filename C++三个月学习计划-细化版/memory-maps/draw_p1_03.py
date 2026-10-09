#!/usr/bin/env python3
"""用 PyMuPDF 画 p1_03_stack_heap.cpp 的内存结构图（风格对齐 p1_01 / p1_02 那两张）。

输出：p1_03_memory_map.pdf / .svg / -N.png（共 3 页）

数据来源（图上每个地址都能对回去）：

1. code/outputs/p1_03_stack_heap.txt —— 本仓库存档的 macOS 实跑输出（Apple clang 21 / arm64），
   脚本直接解析它，所以第 1、2 页的地址与差值同 01-第一阶段-例题讲解.md 的「真实输出」同源。
2. memory-maps/probe_p1_03.cpp —— 换平台复核用的探针。结论是否与平台无关、哪些数值随平台变，
   都靠它给第二组数据：

       c++ -std=c++17 -O0 memory-maps/probe_p1_03.cpp -o /tmp/probe_p1_03 && /tmp/probe_p1_03

排版说明：Song（china-ss）会把西文按全角渲染，混排又宽又空，所以这里按字符分派字体 ——
中文用 china-ss、西文用 helv、地址用 cour，宽度也按同一套规则测量。

用法：python3 memory-maps/draw_p1_03.py [输出目录]
"""
from __future__ import annotations

import math
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
sys.path.insert(0, str(HERE))

import fitz  # noqa: E402

import draw as base  # 复用页面尺寸 / Painter / 基础配色  # noqa: E402

OUT = Path(sys.argv[1]) if len(sys.argv) > 1 else HERE
ARCHIVE = ROOT / "code" / "outputs" / "p1_03_stack_heap.txt"

PAGE_W, PAGE_H = base.PAGE_W, base.PAGE_H
MARGIN_X, MARGIN_TOP, MARGIN_BOTTOM = base.MARGIN_X, base.MARGIN_TOP, base.MARGIN_BOTTOM
CONTENT_W = base.CONTENT_W
CJK = base.FONT     # "china-ss"
LATIN = "helv"      # Helvetica
MONO = "cour"       # Courier
STROKE = base.STROKE
TEXT = base.TEXT
HINT = (0.36, 0.36, 0.36)
DIM = (0.46, 0.46, 0.46)
ACCENT = (0.10, 0.36, 0.62)
ALERT = (0.68, 0.20, 0.14)
WARM = (0.55, 0.30, 0.12)

COLORS = dict(base.COLORS)
COLORS.update({
    "stack": (0.85, 0.90, 0.97),      # 栈上的槽位
    "stack_cur": (0.72, 0.83, 0.95),  # main 栈帧 / 栈上的指针变量
    "heap": (0.90, 0.87, 0.97),       # 堆上的节点
    "global": (0.86, 0.94, 0.87),     # 全局 / 静态区
    "code": (0.93, 0.93, 0.93),       # 代码段
    "reuse": (0.99, 0.94, 0.78),      # scoped / reused 这对
    "unused": (0.955, 0.955, 0.955),  # 本次运行没用到的位置
})
POINTER = (0.45, 0.25, 0.55)


# ------------------------------------------------------------------ 文字层

def _font_of(char: str) -> str:
    """中文（含全角标点、箭头、①、≠）用 Song，其余西文用 Helvetica。"""
    return CJK if ord(char) >= 0x2010 else LATIN


def twidth(text: str, size: float) -> float:
    return sum(fitz.get_text_length(c, fontname=_font_of(c), fontsize=size) for c in text)


def wrap_text(text: str, size: float, width: float) -> list:
    lines, current = [], ""
    for char in text:
        if current and twidth(current + char, size) > width:
            lines.append(current)
            current = char
        else:
            current += char
    if current:
        lines.append(current)
    return lines


def draw_text(p, x: float, y: float, text: str, size: float = 7.4, color=TEXT) -> float:
    """按 run 分段画混排文本（中文 Song / 西文 Helvetica），返回结束位置。"""
    runs: list = []
    for char in text:
        font = _font_of(char)
        if runs and runs[-1][1] == font:
            runs[-1][0] += char
        else:
            runs.append([char, font])
    for run, font in runs:
        p.page.insert_text((x, y), run, fontname=font, fontsize=size, color=color)
        x += fitz.get_text_length(run, fontname=font, fontsize=size)
    return x


def draw_mono(p, x: float, y: float, text: str, size: float = 6.2, color=TEXT) -> float:
    p.page.insert_text((x, y), text, fontname=MONO, fontsize=size, color=color)
    return x + fitz.get_text_length(text, fontname=MONO, fontsize=size)


def para(p, x: float, text: str, size: float = 7.2, width: float = CONTENT_W,
         color=TEXT, line_h: float = 9.2) -> None:
    """从 p.y 往下画一段自动换行的文字，结束后 p.y 停在段落下方。"""
    for line in wrap_text(text, size, width):
        draw_text(p, x, p.y + size, line, size=size, color=color)
        p.y += line_h
    p.y += 2


def bullets(p, items: list, size: float = 7.2) -> None:
    for item in items:
        first = True
        for line in wrap_text(item, size, CONTENT_W - 9):
            if first:
                draw_text(p, MARGIN_X, p.y + size, "·", size=size, color=DIM)
            draw_text(p, MARGIN_X + 9, p.y + size, line, size=size, color=(0.16, 0.16, 0.16))
            p.y += size + 2.2
            first = False
        p.y += 1.4
    p.y += 3.0


# ------------------------------------------------------------------ 图形层

def label_in(p, rect, text: str, size_start: float = 6.8, minimum: float = 4.2,
             color=TEXT, short: str | None = None, mono: bool = False) -> None:
    def measure(value, size):
        if mono:
            return fitz.get_text_length(value, fontname=MONO, fontsize=size)
        return twidth(value, size)

    def fit(value, start):
        size = start
        while size > minimum and measure(value, size) > rect.width - 2.5:
            size -= 0.2
        return size

    size = fit(text, size_start)
    if measure(text, size) > rect.width - 2.5 and short:
        text = short
        size = fit(text, size_start)
    x = rect.x0 + (rect.width - measure(text, size)) / 2
    y = rect.y0 + rect.height / 2 + size * 0.36
    if mono:
        draw_mono(p, x, y, text, size=size, color=color)
    else:
        draw_text(p, x, y, text, size=size, color=color)


def arrow(p, x1: float, y1: float, x2: float, y2: float, color=HINT,
          width: float = 0.7, head: float = 3.4, dash: str | None = None) -> None:
    p.page.draw_line(fitz.Point(x1, y1), fitz.Point(x2, y2), color=color, width=width,
                     dashes=dash)
    angle = math.atan2(y2 - y1, x2 - x1)
    for delta in (math.radians(150), math.radians(-150)):
        p.page.draw_line(
            fitz.Point(x2, y2),
            fitz.Point(x2 + head * math.cos(angle + delta), y2 + head * math.sin(angle + delta)),
            color=color, width=width)


def hatched(p, rect) -> None:
    offset = 4.0
    while offset < rect.height:
        p.page.draw_line(fitz.Point(rect.x0, rect.y0 + offset),
                         fitz.Point(rect.x1, rect.y0 + offset),
                         color=(0.82, 0.82, 0.82), width=0.35)
        offset += 4.0


def band(p, title: str, lines: list, content_h: float, fill: str, inner=None,
         right: str | None = None, right_color=HINT):
    """画一个区域带：标题 + 说明行 + 交给 inner(area) 画的固定高度内容。"""
    p.ensure(content_h + 96)
    wrapped = [line for item in lines for line in wrap_text(item, 7.0, CONTENT_W - 34)]
    height = 22 + 8.6 * len(wrapped) + 4 + content_h + 8
    top = p.y
    rect = fitz.Rect(MARGIN_X, top, PAGE_W - MARGIN_X, top + height)
    p.page.draw_rect(rect, color=(0.55, 0.55, 0.55), fill=COLORS[fill], width=0.7)
    end = draw_text(p, MARGIN_X + 8, top + 12, title, size=9.8)
    if right:
        size = 7.0
        limit = rect.x1 - 8 - end - 12
        while size > 5.6 and twidth(right, size) > limit:
            size -= 0.2
        draw_text(p, rect.x1 - 8 - twidth(right, size), top + 11.5, right, size=size,
                  color=right_color)
    y = top + 22
    for line in wrapped:
        draw_text(p, MARGIN_X + 8, y + 7, line, size=7.0, color=HINT)
        y += 8.6
    area = fitz.Rect(rect.x0 + 8, y + 4, rect.x1 - 8, rect.y1 - 8)
    if inner:
        inner(area)
    p.y = top + height
    return area


def section(p, title: str, note: str | None = None) -> None:
    p.ensure(26)
    p.page.draw_rect(fitz.Rect(MARGIN_X, p.y + 1.5, MARGIN_X + 2.4, p.y + 11),
                     color=(0.55, 0.55, 0.55), fill=(0.72, 0.78, 0.86), width=0)
    end = draw_text(p, MARGIN_X + 6, p.y + 10.5, title, size=9.8)
    if note:
        draw_text(p, end + 7, p.y + 10.2, note, size=7.0, color=HINT)
    p.y += 14.5


def rules_box(p, title: str, items: list) -> None:
    wrapped = [wrap_text("{}. {}".format(i, item), 7.8, CONTENT_W - 22)
               for i, item in enumerate(items, start=1)]
    height = 20.0 + sum(10.6 * len(lines) + 1.6 for lines in wrapped)
    p.ensure(height + 6)
    top = p.y
    p.page.draw_rect(fitz.Rect(MARGIN_X, top, PAGE_W - MARGIN_X, top + height),
                     color=(0, 0, 0), width=0.8)
    draw_text(p, MARGIN_X + 7, top + 13, title, size=10)
    p.y = top + 20
    for lines in wrapped:
        for line in lines:
            draw_text(p, MARGIN_X + 7, p.y + 7, line, size=7.8)
            p.y += 10.6
        p.y += 1.6
    p.y += 8


def legend(p, entries: list) -> None:
    x = MARGIN_X
    for kind, label in entries:
        p.page.draw_rect(fitz.Rect(x, p.y, x + 11, p.y + 8), color=STROKE,
                         fill=COLORS[kind], width=0.5)
        if kind in ("pad", "unused"):
            for offset in (2.5, 5.0):
                p.page.draw_line(fitz.Point(x, p.y + offset), fitz.Point(x + 11, p.y + offset),
                                 color=(0.82, 0.82, 0.82), width=0.4)
        draw_text(p, x + 14, p.y + 6.4, label, size=7.2)
        x += 14 + twidth(label, 7.2) + 14
    p.y += 16


PAGE_TITLE = "p1_03_stack_heap.cpp · 内存结构图"


def page_title(p, title: str, subtitle: str) -> None:
    draw_text(p, MARGIN_X, p.y + 12, title, size=15)
    p.y += 20
    for line in wrap_text(subtitle, 7.6, CONTENT_W):
        draw_text(p, MARGIN_X, p.y + 7, line, size=7.6, color=HINT)
        p.y += 9.8
    p.y += 4


def field_row(p, rect, spans: list, cell_bytes: int = 1) -> None:
    """spans: (标签, 字节数, 配色[, 短标签])，从左到右铺满 rect。"""
    total = sum(span[1] for span in spans)
    cell_w = rect.width / (total / float(cell_bytes))
    x = rect.x0
    for span in spans:
        label, count, kind = span[0], span[1], span[2]
        short = span[3] if len(span) > 3 else None
        width = cell_w * (count / float(cell_bytes))
        cell = fitz.Rect(x, rect.y0, x + width, rect.y1)
        p.page.draw_rect(cell, color=STROKE, fill=COLORS[kind], width=0.5)
        if kind in ("pad", "unused"):
            hatched(p, cell)
        if label:
            label_in(p, cell, label, short=short)
        x += width


def byte_row(p, rect, values: list) -> None:
    cell_w = rect.width / len(values)
    for index, value in enumerate(values):
        x = rect.x0 + cell_w * index
        cell = fitz.Rect(x, rect.y0, x + cell_w, rect.y1)
        p.page.draw_rect(cell, color=(0.80, 0.80, 0.80), fill=(0.985, 0.985, 0.985),
                         width=0.4)
        text = value if value else "??"
        label_in(p, cell, text, size_start=5.4, minimum=3.6, mono=True,
                 color=(0.70, 0.30, 0.22) if not value else (0.25, 0.25, 0.25))


def offset_ruler(p, rect, total_bytes: int, step: int = 4) -> None:
    top = rect.y0
    for value in range(0, total_bytes + 1, step):
        x = rect.x0 + rect.width * value / total_bytes
        p.page.draw_line(fitz.Point(x, top - 3.2), fitz.Point(x, top),
                         color=(0.6, 0.6, 0.6), width=0.4)
        text = "+0x{:x}".format(value)
        if value == total_bytes:
            draw_mono(p, x - fitz.get_text_length(text, fontname=MONO, fontsize=5.0) - 0.6,
                      top - 4.6, text, size=5.0, color=DIM)
        else:
            draw_mono(p, x + 0.6, top - 4.6, text, size=5.0, color=DIM)


def grid_span(p, top: float, x_from: float, x_to: float, text: str, color=DIM) -> None:
    y = top + 4
    p.page.draw_line(fitz.Point(x_from, y), fitz.Point(x_from, y + 3),
                     color=(0.6, 0.6, 0.6), width=0.4)
    p.page.draw_line(fitz.Point(x_to, y), fitz.Point(x_to, y + 3),
                     color=(0.6, 0.6, 0.6), width=0.4)
    p.page.draw_line(fitz.Point(x_from, y + 3), fitz.Point(x_to, y + 3),
                     color=(0.6, 0.6, 0.6), width=0.4)
    draw_text(p, (x_from + x_to) / 2 - twidth(text, 6.4) / 2, y + 11, text, size=6.4,
              color=color)


def steps(p, items: list, box_h: float = 46.0) -> None:
    count = len(items)
    gap = 16.0
    box_w = (CONTENT_W - gap * (count - 1)) / count
    p.ensure(box_h + 6)
    top = p.y
    for index, (title, lines) in enumerate(items):
        x = MARGIN_X + index * (box_w + gap)
        rect = fitz.Rect(x, top, x + box_w, top + box_h)
        p.page.draw_rect(rect, color=(0.72, 0.72, 0.72), fill=(0.975, 0.975, 0.985), width=0.5)
        p.page.draw_rect(fitz.Rect(x, top, x + 2.2, top + box_h), color=(0.55, 0.55, 0.55),
                         fill=(0.72, 0.78, 0.86), width=0)
        draw_text(p, x + 7, top + 11, title, size=7.6)
        y = top + 19
        for line in lines:
            for wrapped in wrap_text(line, 6.8, box_w - 14):
                draw_text(p, x + 7, y + 6.4, wrapped, size=6.8, color=HINT)
                y += 8.6
        if index < count - 1:
            arrow(p, rect.x1 + 2, top + box_h / 2 - 6, rect.x1 + gap - 2, top + box_h / 2 - 6)
    p.y = top + box_h + 6


def _cell_style(cell: str):
    color, use_mono = TEXT, False
    while cell[:1] in "!^$":
        mark, cell = cell[0], cell[1:]
        if mark == "!":
            color = ALERT
        elif mark == "^":
            color = ACCENT
        else:
            use_mono = True
    return cell, color, use_mono


def table(p, headers: list, rows: list, widths: list, size: float = 7.2,
          row_h: float = 13.0, mono_cols: tuple = ()) -> None:
    p.ensure(row_h * (len(rows) + 1) + 8)
    top = p.y
    xs = [MARGIN_X]
    for width in widths:
        xs.append(xs[-1] + width)
    span = xs[-1] - MARGIN_X
    p.page.draw_rect(fitz.Rect(MARGIN_X, top, MARGIN_X + span, top + row_h),
                     color=(0.78, 0.78, 0.78), fill=(0.93, 0.94, 0.96), width=0.4)
    for index, header in enumerate(headers):
        draw_text(p, xs[index] + 3, top + row_h - 4.4, header, size=size - 0.2, color=HINT)
    y = top + row_h
    for row in rows:
        for index, cell in enumerate(row):
            text, color, use_mono = _cell_style(cell)
            if index in mono_cols:
                use_mono = True
            if use_mono:
                draw_mono(p, xs[index] + 3, y + row_h - 4.2, text, size=size - 0.5, color=color)
            else:
                draw_text(p, xs[index] + 3, y + row_h - 4.2, text, size=size - 0.5, color=color)
        p.page.draw_line(fitz.Point(MARGIN_X, y + row_h), fitz.Point(MARGIN_X + span, y + row_h),
                         color=(0.86, 0.86, 0.86), width=0.35)
        y += row_h
    p.y = y + 6


# ------------------------------------------------------------------ 实测数据

def load_archive(path: Path = ARCHIVE) -> dict:
    """解析 code/outputs/p1_03_stack_heap.txt：第 1、2 页的地址全部来自这里。"""
    text = path.read_text(encoding="utf-8")

    def one(pattern: str, name: str) -> int:
        match = re.search(pattern, text)
        if not match:
            raise SystemExit("存档输出里找不到「{}」：{}".format(name, path))
        return int(match.group(1), 16)

    levels = [(int(d), int(a, 16))
              for d, a in re.findall(r"level\((\d)\): 栈变量 &local = (0x[0-9a-f]+)", text)]
    arrays = [(int(i), int(v), int(a, 16))
              for i, v, a in re.findall(r"arr\[(\d)\] 值=(\d+) 地址=(0x[0-9a-f]+)", text)]
    nodes = [(int(v), int(a, 16))
             for v, a in re.findall(r"new Node\{value=(\d+)\} → 堆地址 (0x[0-9a-f]+)", text)]
    freed = [(int(a, 16), int(v))
             for a, v in re.findall(r"delete 堆节点 (0x[0-9a-f]+)（值 (\d+)）", text)]
    abc = re.search(r"&a=(0x[0-9a-f]+) &b=(0x[0-9a-f]+) &c=(0x[0-9a-f]+)", text)
    if not (levels and arrays and nodes and freed and abc):
        raise SystemExit("存档输出格式对不上，解析失败：{}".format(path))

    return {
        "source": path,
        "levels": levels,
        "globals": one(r"g_counter（全局/静态区）地址 = (0x[0-9a-f]+)", "g_counter"),
        "abc": (int(abc.group(1), 16), int(abc.group(2), 16), int(abc.group(3), 16)),
        "array": arrays,
        "nodes": nodes,
        "freed": freed,
        "head": one(r"&head=(0x[0-9a-f]+)", "head"),
        "scoped": one(r"作用域内的 scoped 在 (0x[0-9a-f]+)", "scoped"),
        "reused": one(r"作用域外的 reused 在 (0x[0-9a-f]+)", "reused"),
    }


RUN = load_archive()

# memory-maps/probe_p1_03.cpp 在本机（Linux x86-64 / GCC 16.2）的一次实测结果。
# 在这张图里只干一件事：说明「结论与平台无关，数值与平台有关」。
LINUX = {
    "machine": "Linux x86-64 / GCC 16.2",
    "frame_delta": 0x30,
    "frame_buf_delta": 0x70,
    "node_size": 16,
    "heap_delta": 0x20,
    "stack": 0x7FFCD4EE7804,
    "globals": 0x5EFA41F8E068,
    "heap": 0x5EFA7B69E030,
    "scoped": 0x7FFCD4EE7838,
    "reused": 0x7FFCD4EE7838,
    # node{value=2, next=head} 的 16 个字节；4..7 是填充，内容未定义（这次刚好是 00）
    "raw": ["02", "00", "00", "00", None, None, None, None,
            "70", "e0", "69", "7b", "fa", "5e", "00", "00"],
}


def hx(value: int) -> str:
    return "0x{:x}".format(value)


def delta(a: int, b: int) -> str:
    return "0x{:x}".format(abs(a - b))


def magnitude(value: int) -> str:
    amount = value / float(1024 ** 3)
    if amount >= 1024:
        return "约 {:.0f} TiB".format(amount / 1024)
    return "约 {:.1f} GiB".format(amount)


# ------------------------------------------------------------------ 第 1 页

def page_one(p) -> None:
    levels, nodes = RUN["levels"], RUN["nodes"]
    page_title(p, PAGE_TITLE,
               "数据源：{}（Apple clang 21 / arm64 / macOS 的存档实跑输出）。"
               "地址每次运行都不同 —— 看相对关系，不看具体数值。".format(
                   RUN["source"].relative_to(ROOT)))
    legend(p, [("stack", "栈（自动回收）"), ("heap", "堆（要手动 delete）"),
               ("global", "全局 / 静态区"), ("code", "代码段"),
               ("unused", "本次没用到的位置")])
    p.y += 2

    # ------------------------------------------------ ① 栈
    chip_w = 296.0
    grid_lo = RUN["reused"] - 8
    grid_hi = RUN["array"][-1][2] + 4
    note = ("每往下一层低 {} 字节 = 一个 level 栈帧；level() 返回时这 4 帧一次性回收，"
            "没有任何 delete".format(delta(levels[0][1], levels[1][1])))

    def stack_inner(area):
        main_rect = fitz.Rect(area.x0, area.y0, area.x0 + chip_w, area.y0 + 22)
        p.page.draw_rect(main_rect, color=STROKE, fill=COLORS["stack_cur"], width=0.5)
        draw_text(p, main_rect.x0 + 4, main_rect.y0 + 9, "main 栈帧", size=7.4)
        end = draw_mono(p, main_rect.x0 + 52, main_rect.y0 + 9,
                        "{} - {}".format(hx(grid_lo), hx(grid_hi)), size=6.2, color=HINT)
        draw_text(p, end + 5, main_rect.y0 + 9, "（{} 字节）".format(grid_hi - grid_lo),
                  size=6.4, color=HINT)
        draw_text(p, main_rect.x0 + 4, main_rect.y0 + 18.5,
                  "head / a / b / c / arr[4] / scoped —— 第 2 页逐格画", size=6.6, color=HINT)
        first_y = main_rect.y1 + 6
        for index, (depth, address) in enumerate(levels):
            y = first_y + index * 18
            rect = fitz.Rect(area.x0, y, area.x0 + chip_w, y + 16)
            p.page.draw_rect(rect, color=STROKE,
                             fill=COLORS["stack_cur"] if index == 0 else COLORS["stack"],
                             width=0.5)
            draw_text(p, rect.x0 + 4, y + 11, "level({}) 的 local".format(depth), size=7.2)
            draw_mono(p, rect.x0 + 108, y + 11, "&local = " + hx(address), size=6.4)
            if index:
                draw_text(p, rect.x1 + 26, y + 11, "-" + delta(address, levels[index - 1][1]),
                          size=6.8, color=DIM)
                arrow(p, rect.x1 + 10, y - 3.5, rect.x1 + 10, y + 11, color=(0.62, 0.62, 0.62),
                      width=0.6, head=2.6)
        arrow_x = area.x1 - 10
        arrow(p, arrow_x, first_y + 20, arrow_x, first_y + 3 * 18 + 9, color=(0.30, 0.30, 0.30),
              width=0.8)
        draw_text(p, area.x1 - 18 - twidth("地址由高到低", 6.6), first_y + 12, "地址由高到低",
                  size=6.6, color=DIM)
        draw_text(p, area.x0, first_y + 4 * 18 + 6, note, size=7.0, color=HINT)

    band(p, "① 栈 [stack]",
         ["高地址在上：main 的栈帧先占好位置，它调用的 level(0..3) 依次落在更低的地址上。"],
         22 + 6 + 4 * 18 + 6 + 10, "stack", inner=stack_inner)

    # ------------------------------------------------ 栈与堆的距离
    p.y += 6
    gap_y = p.y + 5
    arrow(p, MARGIN_X + 26, gap_y - 4, MARGIN_X + 26, gap_y + 12, color=(0.72, 0.40, 0.20))
    arrow(p, MARGIN_X + 26, gap_y + 12, MARGIN_X + 26, gap_y - 4, color=(0.72, 0.40, 0.20))
    para(p, MARGIN_X + 38,
         "栈 {} ↔ 堆 / 静态区 {}：相差 {}，是两段互不相干的区域，不是挨着的两块；同一份代码换到 {} "
         "上是 {} —— 差几个数量级这件事是稳的，具体数值不是。".format(
             hx(levels[0][1]), hx(nodes[0][1]), magnitude(levels[0][1] - nodes[0][1]),
             LINUX["machine"], magnitude(LINUX["stack"] - LINUX["heap"])),
         size=7.0, width=CONTENT_W - 38, color=WARM)
    p.y += 2

    # ------------------------------------------------ ② 堆
    box_h, head_w = 30.0, 86.0

    def heap_inner(area):
        head_rect = fitz.Rect(area.x0, area.y0, area.x0 + head_w, area.y0 + box_h)
        p.page.draw_rect(head_rect, color=STROKE, fill=COLORS["stack_cur"], width=0.5)
        draw_text(p, head_rect.x0 + 4, head_rect.y0 + 9.5, "head 变量", size=6.8)
        draw_text(p, head_rect.x0 + 4, head_rect.y0 + 18.5, "（本身在栈上）", size=6.4,
                  color=HINT)
        draw_mono(p, head_rect.x0 + 4, head_rect.y0 + 27, hx(RUN["head"]), size=6.0, color=HINT)
        ordered = sorted(nodes, key=lambda item: item[1])
        x = head_rect.x1 + 24
        box_w = (area.x1 - x - 2 * 22) / 3
        rects = {}
        for value, address in ordered:
            rect = fitz.Rect(x, area.y0, x + box_w, area.y0 + box_h)
            p.page.draw_rect(rect, color=STROKE, fill=COLORS["heap"], width=0.5)
            draw_text(p, rect.x0 + 4, rect.y0 + 9.5, "Node value={}".format(value), size=6.6)
            draw_mono(p, rect.x0 + 4, rect.y0 + 18.5, hx(address), size=6.0, color=HINT)
            draw_text(p, rect.x0 + 4, rect.y0 + 27.5, "第 {} 次 new".format(value + 1),
                      size=6.2, color=DIM)
            rects[value] = rect
            x = rect.x1 + 22
        middle = area.y0 + box_h / 2
        top_node = rects[max(rects)]
        arrow(p, head_rect.x1 + 2, middle, top_node.x0 - 2, middle, color=POINTER, width=0.8)
        draw_text(p, (head_rect.x1 + top_node.x0) / 2 - twidth("next", 6.2) / 2, middle - 4,
                  "next", size=6.2, color=POINTER)
        for value in sorted(rects, reverse=True):
            if value - 1 not in rects:
                continue
            a, b = rects[value], rects[value - 1]
            arrow(p, a.x1 + 2, middle, b.x0 - 2, middle, color=POINTER, width=0.8)
        draw_mono(p, rects[min(rects)].x1 + 5, middle + 2, "null", size=6.0, color=POINTER)

    band(p, "② 堆（new / malloc 的区）",
         ["按地址从小到大摆好这三块：第 3 次 new 反而落到了最低地址 —— 分配顺序 ≠ 地址顺序，"
          "间隔和顺序都由分配器定，不是语言保证。"], box_h + 4, "heap", inner=heap_inner)
    p.y += 2
    para(p, MARGIN_X,
         "存档这次运行：node0 {}、node1 {}（间隔 {}），node2 {} 比 node1 低 {}；本机 glibc 则是"
         "间隔 {} 且一路递增。两边的共同点只有一个：每块 Node 都是 {} 字节，谁 new 谁 delete。".format(
             hx(nodes[0][1]), hx(nodes[1][1]), delta(nodes[0][1], nodes[1][1]),
             hx(nodes[2][1]), delta(nodes[1][1], nodes[2][1]), hx(LINUX["heap_delta"]),
             LINUX["node_size"]),
         size=7.0, color=HINT)
    p.y += 2

    # ------------------------------------------------ ③ 全局 / 静态区、④ 代码段
    def global_inner(area):
        rect = fitz.Rect(area.x0, area.y0, area.x0 + 240, area.y0 + 18)
        p.page.draw_rect(rect, color=STROKE, fill=COLORS["global"], width=0.5)
        draw_mono(p, rect.x0 + 4, rect.y0 + 12.5, "static int g_counter = " + hx(RUN["globals"]),
                  size=6.6)
        draw_text(p, rect.x1 + 12, rect.y0 + 12.5,
                  "和堆同在 {} 那一段，离栈 {}".format(hx(RUN["globals"])[:7] + "…",
                                                  magnitude(RUN["levels"][0][1] - RUN["globals"])),
                  size=6.8, color=HINT)

    band(p, "③ 全局 / 静态区", [], 18, "global", inner=global_inner,
         right="生命周期 = 整个进程，不受函数返回影响")
    p.y += 3
    band(p, "④ 代码段（可执行文件本体，只读）",
         ["main / level / Node 的机器码躺在地址最低的一端：new 和 delete 都是这里发出的指令，"
          "指令本身不占栈也不占堆。"], 0, "code")
    p.y += 10

    section(p, "这道例题的四段路", "按源码顺序读，每一段都对应上面的一块区域")
    table(p, ["源码（code/p1_03_stack_heap.cpp）", "这一段想让你看见什么"], [
        ("level(0) → level(3)（19–20 行）", "栈向下长：每层一个固定栈帧，返回时整段回收"),
        ("g_counter / a b c / arr（21–31 行）", "三段区间互不相干；数组元素必须连续，独立变量不保证"),
        ("new Node ×3（34–39 行）", "堆的地址和顺序由分配器定，new 只保证「给你 16 字节」"),
        ("scoped / reused / delete（44–58 行）", "栈自动复用、堆手动释放：谁 new 谁 delete"),
    ], [190, 323])
    p.y += 4

    rules_box(p, "第 1 页小结", [
        "栈向下长，每层一个固定大小的栈帧：存档里四层 level 每层稳差 {} 字节；函数返回时整段一次性回收，"
        "不需要写任何 delete。".format(delta(levels[0][1], levels[1][1])),
        "堆的地址、间隔、甚至先后顺序都不是语言保证：存档这次运行里第 3 次 new 落到了最低地址，"
        "所以只记住「谁 new 谁 delete」，别记住地址长什么样。",
        "栈、堆、静态区是三段互不相干的区域，它们之间唯一的接口是「指针 + 生命周期」——"
        "内存出问题，基本都出在这条接口上（第 3 页）。",
    ])


# ------------------------------------------------------------------ 第 2 页

def page_two(p) -> None:
    reused, scoped, head = RUN["reused"], RUN["scoped"], RUN["head"]
    a, b, c = RUN["abc"]
    cell = 4
    start = reused - 2 * cell
    count = (RUN["array"][-1][2] + cell - start) // cell
    cell_w = CONTENT_W / count

    def index(address: int) -> int:
        return (address - start) // cell

    page_title(p, PAGE_TITLE + "（2/3）· main 的栈帧",
               "同一个函数里的变量全在 main 的栈帧里。下面从 {} 起、4 字节一格画出来，格子里的地址"
               "来自存档的同一次运行。".format(hx(start)))
    legend(p, [("stack", "普通局部变量"), ("reuse", "作用域一结束就被复用"),
               ("heap", "指针变量（指向堆）"), ("unused", "这次没打印到的位置")])

    section(p, "main 的栈帧俯视图",
            "4 字节一格，共 {} 格 = {} 字节".format(count, count * cell))
    top = p.y + 7
    entries = {index(reused): ("reused", "reuse"), index(scoped): ("scoped", "reuse"),
               index(head): ("head", "heap"), index(a): ("a", "stack"),
               index(b): ("b", "stack"), index(c): ("c", "stack"),
               index(head) + 1: ("", "heap")}
    for i, _value, address in RUN["array"]:
        entries[index(address)] = ("arr[{}]".format(i), "stack")
    for i in range(count):
        x = MARGIN_X + cell_w * i
        label, kind = entries.get(i, ("", "unused"))
        rect = fitz.Rect(x, top, x + cell_w, top + 22)
        p.page.draw_rect(rect, color=(0.78, 0.78, 0.78), fill=COLORS[kind], width=0.4)
        if kind == "unused":
            hatched(p, rect)
        if label:
            label_in(p, rect, label, size_start=6.8)
    offset_ruler(p, fitz.Rect(MARGIN_X, top, MARGIN_X + CONTENT_W, top), count * cell, 8)
    p.y = top + 22 + 2
    grid_span(p, p.y, MARGIN_X + cell_w * index(reused), MARGIN_X + cell_w * (index(scoped) + 1),
              "scoped / reused 用同一块位置")
    grid_span(p, p.y, MARGIN_X + cell_w * index(head), MARGIN_X + cell_w * (index(head) + 2),
              "head：8 字节指针")
    grid_span(p, p.y, MARGIN_X + cell_w * index(RUN["array"][0][2]),
              MARGIN_X + cell_w * (index(RUN["array"][-1][2]) + 1), "arr[0..3]：连续")
    p.y += 18
    bullets(p, [
        "arr[0..3] 四格紧挨着、步长恰好 4 = sizeof(int)：数组是一个完整的对象，元素必须连续存放。",
        "a / b / c 也挨着，但谁在高位完全由编译器决定：这份存档是递减的（ {} > {} > {} ），"
        "本机 GCC 却是递增的（a < b < c）—— 别按声明顺序推地址。".format(hx(a), hx(b), hx(c)),
        "head 占两格：它是个 8 字节指针，自己老老实实待在栈上，指向的对象在堆上 —— "
        "「变量」和「变量指向的东西」不一定在同一个区。",
        "灰色格子是这次运行没打印到的位置：对齐和调用约定要给临时值留空档，"
        "所以栈帧里总有不属于任何变量的字节。",
        "reused 落在 scoped 用过的位置上（存档里两者相邻 4 字节，本机 GCC 实测地址完全相同）："
        "作用域一结束，那块栈空间马上被别人接手。",
    ])

    section(p, "递归：每层一个固定大小的栈帧", "同一个 level() 反复调用，地址一路往下走")
    rows = []
    for i, (depth, address) in enumerate(RUN["levels"]):
        rows.append(("level({})".format(depth), "$" + hx(address),
                     "$—" if i == 0 else "$-" + delta(address, RUN["levels"][i - 1][1]),
                     "最先调用，地址最高" if i == 0 else "低一个栈帧"))
    table(p, ["调用层", "&local", "与上一层差", "说明"], rows, [70, 112, 82, 263],
          mono_cols=(0, 1, 2))
    bullets(p, [
        "每调用一层，栈指针往下走 {} 字节：一个 int + 保存的返回地址 / 寄存器 + 对齐。".format(
            delta(RUN["levels"][0][1], RUN["levels"][1][1])),
        "动手改一改：给 level() 加一个 char buf[64]，本机实测每层差从 {} 变成 {} = 112 字节"
        "—— 栈帧大小 = 局部变量之和（对齐之后）。".format(
            hx(LINUX["frame_delta"]), hx(LINUX["frame_buf_delta"])),
        "四层一共才 0xc0 字节；把深度换成十万量级就是栈溢出（第三阶段例题 3-3 的伏笔）。",
    ])

    section(p, "生命周期：作用域一结束，地址就被接管")
    steps(p, [
        ("① 进作用域", ["{ int scoped = 42; }", "落在 {}：此刻它是这块栈空间的主人".format(hx(scoped))]),
        ("② 出作用域", ["没有任何指令去「回收」它", "栈指针挪回原位就够了 —— 全自动的那一半"]),
        ("③ 下一个变量接手", ["int reused = 7;", "落在 {}：同一块空间被重新分配".format(hx(reused))]),
    ])
    bullets(p, [
        "栈的「释放」不是清空内存，只是把栈指针挪回去：值还躺在那儿，指针还在你手上，那就是野指针。",
        "同一份代码换到本机 GCC，scoped 和 reused 实测落在完全相同的地址 —— 复用不是巧合，是规则。",
    ])

    section(p, "动手改一改", "先预测，再跑，最后回到这张图上对一对")
    bullets(p, [
        "给 level() 加一个 char buf[64]：先算出每层差会变成多少（提示：向上取整到 16 的倍数），"
        "再跑 —— 本机实测是 {} = 112 字节。".format(hx(LINUX["frame_buf_delta"])),
        "把三次 new Node 改成 new Node[3]：堆上从 3 次分配变成 1 次、地址一定连续，"
        "释放也从 3 次 delete 变成 1 次 delete[] —— 这是唯一能「保证连续」的写法。",
        "把 a / b / c 的声明顺序改一改，看地址跟不跟着换：验证「顺序由编译器定」，"
        "而不是「按声明顺序排」。",
    ])


# ------------------------------------------------------------------ 第 3 页

def page_three(p) -> None:
    nodes, freed = RUN["nodes"], RUN["freed"]
    page_title(p, PAGE_TITLE + "（3/3）· 堆上的 Node 与释放",
               "堆这一侧的三个问题：一个 Node 占多少字节、三者怎么连起来、谁负责把它还回去。")
    legend(p, [("member", "数据成员"), ("pad", "对齐填充（内容未定义）"),
               ("vptr", "指针字段"), ("heap", "堆上的对象")])

    section(p, "Node 的 16 字节", "8 字节的数据 + 8 字节的代价（存档与本机实测一致）")
    top = p.y + 8
    field_row(p, fitz.Rect(MARGIN_X, top, MARGIN_X + CONTENT_W, top + 22),
              [("int value", 4, "member"), ("填充", 4, "pad", ""),
               ("Node *next（8 字节指针）", 8, "vptr", "Node *next")])
    offset_ruler(p, fitz.Rect(MARGIN_X, top, MARGIN_X + CONTENT_W, top), 16, 4)
    p.y = top + 22 + 6
    byte_top = p.y + 8
    byte_row(p, fitz.Rect(MARGIN_X, byte_top, MARGIN_X + CONTENT_W, byte_top + 14), LINUX["raw"])
    offset_ruler(p, fitz.Rect(MARGIN_X, byte_top, MARGIN_X + CONTENT_W, byte_top), 16, 4)
    p.y = byte_top + 14 + 6
    para(p, MARGIN_X,
         "下排是本机 dump 出来的 16 个原始字节（小端）：02 00 00 00 是 value，紧跟着的 8 个字节"
         "是指向下一节点的地址；中间 4 个 ?? 是填充，内容未定义，这次刚好是 00。", size=7.0,
         color=HINT)
    p.y += 2
    bullets(p, [
        "int value 在偏移 0；next 是 8 字节指针、要求 8 对齐，所以 4..7 只能当填充 —— "
        "和例题 1-2 的填充是同一件事，只不过这次它让每个 Node 从 12 字节变成 16 字节。",
        "结构体布局由 ABI 定，跨平台反而很稳：本机 GCC 与存档的 clang 都是 16 字节、"
        "value 在 0、next 在 8。",
    ])

    section(p, "分配 → 链表 → 释放：每一步都不按地址来", "存档这一次运行的完整顺序")
    rows = [
        ("new #1", "node0：value=0，next=nullptr（head 当时还是空）", hx(nodes[0][1])),
        ("new #2", "node1：value=1，next=node0", hx(nodes[1][1])),
        ("new #3", "!node2：value=2，next=node1，地址反而最低", "!" + hx(nodes[2][1])),
        ("delete #1", "先存下 head->next，再 delete", hx(freed[0][0])),
        ("delete #2", "head 前移到 node1，重复上一步", hx(freed[1][0])),
        ("delete #3", "head 变成 nullptr，循环结束", hx(freed[2][0])),
    ]
    table(p, ["步骤", "发生了什么", "地址"], rows, [64, 349, 114], mono_cols=(2,))
    draw_mono(p, MARGIN_X + 3, p.y + 7,
              "while (head) { Node *next = head->next; delete head; head = next; }", size=7.0)
    p.y += 13
    bullets(p, [
        "必须先存 next：delete head 之后再读 head->next 就是读已释放内存"
        "（ASan 会报 heap-use-after-free，第三阶段例题 3-4）。",
        "释放顺序 2 → 1 → 0，正好是 new 的倒序；栈上那些变量不用管，main 一返回全消失，"
        "堆上这三个必须有人 delete —— 漏一个就是泄漏。",
    ])

    section(p, "换一台机器复核：结论稳、数值不稳",
            "左列是仓库存档（macOS / arm64），右列是本机跑 probe_p1_03.cpp 的结果")
    rows = [
        ("递归每层差", "^" + hx(RUN["levels"][0][1] - RUN["levels"][1][1]),
         "^" + hx(LINUX["frame_delta"])),
        ("加 char buf[64] 后每层差", "未测", hx(LINUX["frame_buf_delta"])),
        ("Node 大小 / 偏移", "16 字节，value@0、next@8", "16 字节，value@0、next@8"),
        ("三块堆的间隔与顺序", "{}，第 3 块回到更低地址".format(hx(nodes[1][1] - nodes[0][1])),
         "{}，单调递增".format(hx(LINUX["heap_delta"]))),
        ("a / b / c 地址顺序", "递减（a 最高）", "递增（a 最低）"),
        ("scoped 与 reused", "相邻 4 字节", "地址完全相同"),
        ("栈 ↔ 静态区距离", magnitude(RUN["levels"][0][1] - RUN["globals"]),
         magnitude(LINUX["stack"] - LINUX["globals"])),
    ]
    table(p, ["观察项", "macOS / Apple clang 21（存档）", LINUX["machine"] + "（本机）"],
          rows, [104, 212, 211])
    bullets(p, [
        "两列里唯一稳的是「结构」：栈向下、每层一个固定栈帧、Node 16 字节、指针在栈而对象在堆。",
        "数值每次运行都在变（地址、间隔、距离都会被 ASLR 和分配器改掉），所以只能记规律、"
        "不能记数字 —— 这是这道例题最实用的一条。",
    ])

    section(p, "栈 vs 堆：一张对照表", "本例题标题里的「本质区别」就是这六行")
    table(p, ["对比项", "栈", "堆"], [
        ("空间从哪来", "进入函数时由编译器预留", "new / malloc 向分配器申请"),
        ("谁释放、什么时候", "运行时自动：作用域结束 / 函数返回", "你：delete / free，可提前也可延后"),
        ("释放晚了", "——", "!泄漏：长跑程序内存只涨不落"),
        ("释放早了 / 越界", "踩到相邻栈变量，现象随运行改变", "!野指针、double free（例题 3-4、3-5）"),
        ("地址规律", "必然向下，每层一个固定大小的帧", "由分配器决定：可高可低、间隔不定"),
        ("容量上限", "通常 8 MB 量级（本机 ulimit -s = 8192）", "受物理内存 + 交换空间限制"),
    ], [106, 204, 203])
    p.y += 2

    rules_box(p, "三条结论", [
        "栈和堆的差别不是「快慢」，是「谁负责」：栈由编译器和运行时按作用域自动回收，"
        "堆必须由你 delete —— 漏了是泄漏，早删了是野指针。",
        "地址的方向与顺序都不是语言保证：栈一定向下长、每层一个固定栈帧；堆的地址和间隔看分配器，"
        "甚至可能像存档里那样「后分配的反而更低」。",
        "知道这些是为了做对两件事：跨语言传结构体时按 ABI 而不是靠猜来算大小；写宿主层接口时"
        "先想清楚「谁拥有、活到什么时候」，再动手写代码。",
    ])


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    p = base.Painter()
    page_one(p)
    p.new_page()
    page_two(p)
    p.new_page()
    page_three(p)

    pdf_path = OUT / "p1_03_memory_map.pdf"
    p.doc.save(pdf_path)
    (OUT / "p1_03_memory_map.svg").write_text(
        "\n".join(pg.get_svg_image(text_as_path=True) for pg in p.doc), encoding="utf-8"
    )
    for index in range(p.doc.page_count):
        pix = p.doc[index].get_pixmap(matrix=fitz.Matrix(150 / 72, 150 / 72))
        pix.save(OUT / "p1_03_memory_map-{}.png".format(index + 1))

    print("pages:", p.doc.page_count)
    print("pdf:", pdf_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
