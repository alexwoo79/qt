# day5-2：C++ 业务核心 + HTML 界面

界面用 HTML/CSS/JS 写，业务逻辑用 C++ 写，两边通过 webview 的函数绑定通信。
窗口和网页渲染交给系统 webview，不依赖 Qt。

这个项目同时是一份**最小架构的参考实例**：目录分层、依赖方向、错误处理策略
都已经定好，可以直接照着开下一个项目。

## 依赖

- CMake ≥ 3.16，以及支持 C++17 的编译器
- GTK 3 + WebKitGTK 4.1（`pkg-config` 能找到 `gtk+-3.0` 与 `webkit2gtk-4.1`）
- 第三方单头文件已随项目内置，不需要额外安装：
  `third_party/webview/webview.h`（webview 0.12.0）、`third_party/nlohmann/json.hpp`（nlohmann/json 3.12.0）

## 目录结构

```
day5-2/
├── CMakeLists.txt                 # 顶层：C++ 标准、输出目录、第三方目标、三个子目录
│
├── core/                          # 业务核心：纯 C++，零 UI、零 IO
│   ├── CMakeLists.txt             #   -> 静态库 day5_core
│   ├── include/day5_2/            #   对外头文件（外部只能看到这里）
│   │   ├── result.h               #     ErrorCode / Error / Result<T>
│   │   └── person.h               #     Person 记录 + PersonService 业务逻辑
│   └── src/
│       ├── person.cpp
│       └── text_utils.h           #   模块内部工具，不对外暴露
│
├── app/                           # 界面外壳：UI + 组装
│   ├── CMakeLists.txt             #   -> 可执行文件 day5_2
│   ├── main.cpp                   #   组装根：把 core 和界面接起来
│   ├── controller.h/.cpp          #   控制器：界面动作 <-> 业务调用
│   ├── presenter.h/.cpp           #   展示层：全部中文文案在这里
│   ├── protocol.h/.cpp            #   协议：JSON 编解码、信封定义
│   └── web/index.html             #   视图（HTML + CSS + JS 单文件）
│
├── tests/                         # 脱离界面的业务测试
│   ├── CMakeLists.txt
│   └── person_test.cpp
│
└── third_party/                   # 内置依赖
    ├── webview/webview.h
    └── nlohmann/json.hpp
```

## 四条规则

这四条是整个架构的本体，目录只是它们的表现形式。

**规则一：依赖只朝一个方向。** `app → core`，永远不反向。
`core` 不 include webview、不碰界面、不写文件；`app` 负责把业务结果变成界面能用的东西。
唯一的例外是 `main.cpp`，它是组装根，可以同时看到两边。

这条不靠自觉，靠构建系统保证：`day5_core` 的链接列表里没有 GTK、没有 webview，
所以谁在 `core` 里 include 界面头文件，**编译会直接失败**。

**规则二：一个目录一种角色。**

| 目录 | 只放 | 绝对不放 |
|---|---|---|
| `core/` | 业务规则、数据结构 | 界面代码、JSON、日志输出 |
| `app/` | 界面、渲染、组装 | 业务规则、校验逻辑 |
| `tests/` | 测试 | 被产品代码包含的东西 |

配套命名法：文件名 = 主类型名，全小写下划线；命名空间 = `day5_2`（整个项目只用这一层，
不按 `app`、`core` 再嵌套——层级已经由目录和 include 路径表达了）；
include 一律从项目根写全路径（`#include "day5_2/person.h"`），不用 `./` 或 `../`。

**规则三：边界只交换两种东西。** 进去的是**类型化的参数**，出来的是 `Result<T>`。
`core` 从不产出给用户看的句子，它返回机器可读的失败标识（如 `person.name.empty`），
中文文案由 `app/presenter.cpp` 一处负责。换语言、换图标只改那一个文件。

**规则四：错误处理策略第一天就定死。**

- **可预期的失败**（用户没填姓名、年龄越界）→ 返回值 `Result<T>`
- **不可恢复的失败**（构造不出对象、文件打不开）→ 抛异常

另有一条硬约束：**异常绝对不许越过 C 接口边界**。webview 的绑定回调是 C 接口，
从里面抛异常是未定义行为，所以这一层的解析一律用 `json::parse(args, nullptr, false)`
这种不抛异常的写法。

## 线上协议

页面加载后，webview 把三个函数注入到 JS 的 `window` 上，调用后返回 Promise：

```js
window.getState()                  // 查询状态
window.submitEntry(name, age)      // 提交一条记录
window.clearMessages()             // 清空
```

返回的 JSON 信封（字段名就是前后端契约）：

```json
{
  "ok": false,
  "level": "error",
  "event": "failed",
  "status": "❌ 年龄必须在 1-150 之间",
  "detail": "年龄必须在 1-150 之间",
  "error": "person.age.out_of_range",
  "messages": ["张三 (20 岁)"]
}
```

| 字段 | 含义 |
|---|---|
| `ok` | 本次操作是否成功 |
| `level` | `info` 中性 / `ok` 成功 / `error` 失败，界面据此决定样式 |
| `event` | `state` / `submitted` / `failed` / `cleared` |
| `status` | 状态栏文本，可以直接显示 |
| `detail` | 本条记录的摘要，或失败说明 |
| `error` | 失败时机器可读的原因；成功为空 |
| `messages` | 列表内容（已格式化） |

界面只认 `ok` 和 `level`，**不去嗅探 ✅❌ 这类文本前缀**——文案可以在后端随便改，
界面不受影响。需要精确处理某个错误时，用 `error` 字段判分支。

## 加一个新业务，要动哪些文件

以"新增一个订单模块"为例，固定就这几步：

1. `core/include/day5_2/<业务>.h` —— 数据结构 + 业务类声明
2. `core/src/<业务>.cpp` —— 业务规则，失败返回 `Result` + `Error{code, key}`
3. `core/CMakeLists.txt` 里把新 `.cpp` 加进 `day5_core`
4. `tests/<业务>_test.cpp` + `tests/CMakeLists.txt` —— 先测业务，再接界面
5. `app/presenter.cpp` 的 `lookup` 里给新 key 加中文（多数情况复用已有错误码，不用改）
6. `app/controller.cpp` 里注册一行绑定

第 5、6 步通常只有几行。判断这个架构是否好用的标准就是：**加业务不需要改架构代码。**

## 构建与运行

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/bin/day5_2          # 需要图形环境
```

跑测试（不需要图形环境）：

```bash
cd build && ctest --output-on-failure
```

## 若干说明

- **依赖方向可验证**：`day5_core` 只暴露 `core/include/`，链接列表里没有任何界面库。
  想确认架构没被破坏，看一眼 `core/CMakeLists.txt` 就够了。
- **头文件路径**：公开头文件放在 `include/day5_2/` 下，按 `#include "day5_2/xxx.h"` 引用；
  只在本模块内部使用的头文件（如 `src/text_utils.h`）留在 `src/` 里，不进 `include/`。
  这样"哪个文件是给别人用的"一目了然。
- **`Result<T>` 不是自创**：C++23 已经把它标准化为 `std::expected<T, E>`，
  等编译器支持了直接替换，业务代码不用改。
- **JSON 编解码用 `nlohmann/json`**：顶层 CMakeLists 把它包成 INTERFACE 目标
  `nlohmann_json::nlohmann_json`，用法与官方 `find_package` 一致，以后换系统包时链接处不用改。
- **回包用 `ordered_json`**：默认的 `json` 底层是 `std::map`，`dump()` 会按 key 字母序输出；
  `ordered_json` 保持写入顺序，读日志更直观。
- **HTML 在构建期内嵌**：CMake 把 `app/web/index.html` 转成 `index_html.h`，
  运行时不需要额外文件，改完 HTML 要重新构建。
  想边改边看，把 `main.cpp` 里的 `window.set_html(kIndexHtml)` 换成
  `window.navigate("file:///绝对路径/app/web/index.html")`。
- **`index.html` 里不要写 CMake 的 `"${...}"` 形式**，否则会在配置阶段被展开。
- 当前使用 webview 0.12.0（`webview::webview` 是 `browser_engine` 的别名）+ GTK 3 + WebKitGTK 4.1。

## 什么时候需要加目录

目录随需要增长，不提前建空目录。等哪天遇到下面这些情况再动：

| 触发条件 | 加什么 |
|---|---|
| 出现第二种界面（比如同时要 Qt 版） | `app/<壳名>/` 拆目录，`presenter`/`protocol` 提为共用 |
| `core` 里出现第二个业务域 | `core/include/day5_2/<域>/` 分子目录 |
| 存储要换实现（内存 → SQLite） | 抽 `<域>_repository.h` 接口，实现放 `app/` |
| 出现第二套传输（REST/CLI） | 抽 `presenter`/`protocol` 的公共部分 |
