# ledger：记账本（模板实战示例）

用 `cpp-app-template` 的骨架做的一个小应用：记一笔开销，自动算合计。
C++ 写业务，HTML 写界面，两边通过 webview 的函数绑定通信。

这个项目是用来**走一遍模板流程**的实例——从模板生成、换业务、加一个协议字段，
全程只改了"该改的地方"，架构代码一行没动。

## 一句话架构

**两层（core / app）+ 一条单向依赖 + 四条规则。**

```
core/  业务核心：纯 C++，零 UI、零 IO     ← 金额、校验、合计在这里
app/   界面外壳：UI + 组装 + 文案         ← 元/分的写法、中文文案在这里
```

## 目录结构

```
ledger/
├── CMakeLists.txt                 # 顶层：C++ 标准、输出目录、第三方目标、三个子目录
│
├── core/                          # 业务核心：纯 C++，零 UI、零 IO
│   ├── CMakeLists.txt             #   -> 静态库 ledger_core
│   ├── include/ledger/            #   对外头文件（外部只能看到这里）
│   │   ├── result.h               #     ErrorCode / Error / Result<T>
│   │   └── ledger.h               #     Entry 记录 + LedgerService 业务逻辑
│   └── src/
│       ├── ledger.cpp
│       └── text_utils.h           #   模块内部工具，不对外暴露
│
├── app/                           # 界面外壳：UI + 组装
│   ├── CMakeLists.txt             #   -> 可执行文件 ledger
│   ├── main.cpp                   #   组装根：把 core 和界面接起来
│   ├── controller.h/.cpp          #   控制器：界面动作 <-> 业务调用
│   ├── presenter.h/.cpp           #   展示层：全部中文文案、元/分换算在这里
│   ├── protocol.h/.cpp            #   协议：JSON 编解码、信封定义
│   └── web/index.html             #   视图（HTML + CSS + JS 单文件）
│
└── tests/                         # 脱离界面的业务测试（不需要图形环境）
    ├── CMakeLists.txt
    └── ledger_test.cpp
```

## 这个项目的两个设计决定

**一、金额用整数"分"存，不用浮点数。**

`core` 里的 `Entry::amount_cents` 是 `int`，代表多少分。钱不能用 `double` 表示，
否则 0.1 + 0.2 那类误差会在合计里冒出来。"元"只是给人看的写法，
属于展示层，由 `app/presenter.cpp` 的 `formatCents` 负责换算。

浮点数只在最外层出现一次：界面输入"12.34 元"，`protocol.cpp` 用
`std::lround(yuan * 100.0)` 换算成 `1234` 分，之后一路都是整数。
测试里专门验了这一点——12.34 + 0.66 的合计正好是 13.00，没有误差。

**二、列表传的是结构化数据，不是拼好的句子。**

信封里的 `entries` 是 `[{name, amount}]`，界面自己排版（名称靠左、金额靠右）。
这和"core 只给事实、展示层负责文本"是同一条规则：后端给数据，怎么摆是界面的事。
需要改布局时不用动 C++。

## 四条规则

这四条是整个架构的本体，目录只是它们的表现形式。

### 规则一：依赖只朝一个方向

`app → core`，永远不反向。`core` 不 include 界面头文件、不碰窗口、不写文件、
不打印日志；`app` 负责把业务结果变成界面能用的东西。
唯一的例外是 `main.cpp`，它是组装根，可以同时看到两边。

**这条靠构建系统强制，不靠自觉**：`ledger_core` 的链接列表里没有 GTK、没有 webview，
谁在 `core` 里 include 界面头文件，编译直接失败。

### 规则二：一个目录一种角色

| 目录 | 只放 | 绝对不放 |
|---|---|---|
| `core/` | 业务规则、数据结构 | 界面代码、JSON、日志输出 |
| `app/` | 界面、渲染、组装 | 业务规则、校验逻辑 |
| `tests/` | 测试 | 被产品代码包含的东西 |
| `third_party/` | 外部代码原样 | 你的改动 |

配套命名法：文件名 = 主类型名（`ledger.h`）；命名空间 = 项目名（`namespace ledger`，
整个项目只用这一层，不按 `app`、`core` 再嵌套——层级已经由目录和 include 路径表达了）；
include 从项目根写全路径（`#include "ledger/ledger.h"`），不用 `./` 或 `../`。

### 规则三：边界只交换两种东西

进去的是**类型化参数**，出来的是 `Result<T>`。
`core` 从不产出给用户看的句子——它返回机器可读的失败标识（如 `entry.name.empty`），
中文文案由 `app/presenter.cpp` 一处负责。

### 规则四：错误处理策略第一天就定死

- **可预期的失败**（名称为空、金额不是正数）→ 返回值 `Result<T>`
- **不可恢复的失败**（构造不出对象、文件打不开）→ 抛异常

另有一条硬约束：**异常绝对不许越过 C 接口边界**。webview 的绑定回调是 C 接口，
从里面抛异常是未定义行为，所以那一层一律用不抛异常的写法
（`json::parse(args, nullptr, false)`）。

## 线上协议

```js
window.getState()             // 查询状态
window.addEntry(name, amount) // 记一笔，amount 是"元"（可以带小数）
window.clearEntries()         // 清空
```

返回的 JSON 信封：

```json
{
  "ok": true,
  "level": "ok",
  "event": "state",
  "status": "还没有记账",
  "detail": "",
  "error": "",
  "total": "合计 13.00 元",
  "entries": [
    { "name": "午餐", "amount": "12.34 元" },
    { "name": "咖啡", "amount": "0.66 元" }
  ]
}
```

| 字段 | 含义 |
|---|---|
| `ok` | 本次操作是否成功 |
| `level` | `info` 中性 / `ok` 成功 / `error` 失败，界面据此决定样式 |
| `event` | `state` / `added` / `failed` / `cleared` |
| `status` | 状态栏文本，可以直接显示 |
| `detail` | 本条记录摘要，或失败说明 |
| `error` | 失败时机器可读的原因；成功为空 |
| `total` | 合计金额（已格式化） |
| `entries` | 列表数据（结构化，界面自己排版） |

界面只认 `ok` 和 `level`，**不去嗅探 ✅❌ 这类文本前缀**。

## 加业务功能的固定动作

这个项目本身就是按这套动作从模板长出来的：

1. `core/include/ledger/ledger.h` —— 数据结构 + 业务类声明
2. `core/src/ledger.cpp` —— 业务规则，失败返回 `Result` + `Error{code, key}`
3. `core/CMakeLists.txt` 里把新 `.cpp` 加进 `ledger_core`
4. `tests/ledger_test.cpp` —— **先测业务，再接界面**
5. `app/presenter.cpp` 的 `lookup` 里给新 key 加中文
6. `app/controller.cpp` 里注册一行 `bind`
7. 需要的话，在 `app/web/index.html` 加控件

这次比模板多做了第八步：**给信封加了一个 `total` 字段，把 `messages`
换成结构化的 `entries`**。协议要长，就是这么长的——只在 `protocol.h` /
`protocol.cpp` 里改形状，`core` 完全不用动。

## 依赖

- CMake ≥ 3.16，支持 C++17 的编译器
- GTK 3 + WebKitGTK 4.1（`pkg-config` 能找到 `gtk+-3.0` 与 `webkit2gtk-4.1`）
- 第三方单头文件已内置：`third_party/webview/webview.h`、`third_party/nlohmann/json.hpp`

## 构建、运行、测试

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/bin/ledger                      # 需要图形环境
cd build && ctest --output-on-failure   # 不需要图形环境
```

测试覆盖：名称空/金额非正数被拒、1 分钱的下界、名称去空白、金额按分保存、
合计无浮点误差、清空后合计归零。

## 若干说明

- **HTML 在构建期内嵌**：CMake 把 `app/web/index.html` 转成 `index_html.h`，
  运行时不需要额外文件，改完 HTML 要重新构建。想边改边看，把 `main.cpp` 里的
  `window.set_html(kIndexHtml)` 换成
  `window.navigate("file:///绝对路径/app/web/index.html")`。
- **`index.html` 里不要写 CMake 的 `"${...}"` 形式**，否则会在配置阶段被展开。
- **回包用 `ordered_json`**：默认的 `json` 底层是 `std::map`，`dump()` 会按 key 字母序输出；
  `ordered_json` 保持写入顺序，读日志更直观。
- **`Result<T>` 不是自创**：C++23 已把它标准化为 `std::expected<T, E>`，
  等编译器支持了直接替换，业务代码不用改。

## 这套约定是抄谁的

| 出处 | 取哪一条 |
|---|---|
| C++ Core Guidelines | E.1 错误策略先定、E.2/E.3 异常用法、R.1/R.11 RAII、I.1 接口显式 |
| Google C++ Style Guide | 命名规范、include 从项目根写全 |
| A Philosophy of Software Design | 深模块、信息隐藏、注释写"为什么" |
| Clean Architecture | 只取"依赖指向内"这一条 |
