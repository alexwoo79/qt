# cpp-app-template：小项目的最小 C++ 架构模板

一份可以直接复制、立刻开始写业务的项目骨架：**C++ 写业务，HTML 写界面**，
两边通过 webview 的函数绑定通信，不依赖 Qt 之类的重型框架。

模板里的待办事项（`Task`）只是一个**示例**。你的业务是订单就换成 `Order`，
是设备就换成 `Device`——**换掉 core 里的业务类型，架构部分一行都不用改**。

## 一句话架构

**两层（core / app）+ 一条单向依赖 + 四条规则。**

```
core/  业务核心：纯 C++，零 UI、零 IO     ← 你的业务写在这里
app/   界面外壳：UI + 组装 + 文案
```

判断这个架构是否好用的标准只有一条：
**新增一个业务功能，不需要修改架构代码。**

## 开新项目

```bash
./scripts/new-project.sh ~/work/pomodoro pomodoro
cd ~/work/pomodoro
cmake -S . -B build -G Ninja && cmake --build build
cd build && ctest --output-on-failure
```

脚本会把模板里的 `myapp` 全部换成你的项目名（文件名、目录名、C++ 命名空间、
可执行文件名一起换）。项目名要求小写字母开头，只用字母/数字/下划线——它会当 C++ 命名空间用。

不想用脚本也可以：整目录复制，然后把 `myapp` 手动替换掉。

## 目录结构

```
myapp/
├── CMakeLists.txt                 # 顶层：C++ 标准、输出目录、第三方目标、三个子目录
│
├── core/                          # 业务核心：纯 C++，零 UI、零 IO
│   ├── CMakeLists.txt             #   -> 静态库 myapp_core
│   ├── include/myapp/             #   对外头文件（外部只能看到这里）
│   │   ├── result.h               #     ErrorCode / Error / Result<T>
│   │   └── task.h                 #     业务数据 + 业务类
│   └── src/
│       ├── task.cpp
│       └── text_utils.h           #   模块内部工具，不对外暴露
│
├── app/                           # 界面外壳：UI + 组装
│   ├── CMakeLists.txt             #   -> 可执行文件 myapp
│   ├── main.cpp                   #   组装根：把 core 和界面接起来
│   ├── controller.h/.cpp          #   控制器：界面动作 <-> 业务调用
│   ├── presenter.h/.cpp           #   展示层：全部中文文案在这里
│   ├── protocol.h/.cpp            #   协议：JSON 编解码、信封定义
│   └── web/index.html             #   视图（HTML + CSS + JS 单文件）
│
├── tests/                         # 脱离界面的业务测试（不需要图形环境）
│   ├── CMakeLists.txt
│   └── task_test.cpp
│
├── scripts/new-project.sh         # 用本模板生成新项目
└── third_party/                   # 内置依赖（webview、nlohmann/json）
```

## 四条规则

这四条是整个架构的本体，目录只是它们的表现形式。

### 规则一：依赖只朝一个方向

`app → core`，永远不反向。`core` 不 include 界面头文件、不碰窗口、不写文件、
不打印日志；`app` 负责把业务结果变成界面能用的东西。
唯一的例外是 `main.cpp`，它是组装根，可以同时看到两边。

**这条靠构建系统强制，不靠自觉**：`myapp_core` 的链接列表里没有 GTK、没有 webview，
谁在 `core` 里 include 界面头文件，编译直接失败。

### 规则二：一个目录一种角色

| 目录 | 只放 | 绝对不放 |
|---|---|---|
| `core/` | 业务规则、数据结构 | 界面代码、JSON、日志输出 |
| `app/` | 界面、渲染、组装 | 业务规则、校验逻辑 |
| `tests/` | 测试 | 被产品代码包含的东西 |
| `third_party/` | 外部代码原样 | 你的改动 |

配套命名法：

- 文件名 = 主类型名，全小写下划线：`task_service.h/.cpp`
- 命名空间 = 项目名：`namespace myapp { ... }`
- include 一律从项目根写全路径：`#include "myapp/task.h"`，不用 `./` 或 `../`
- 文件/类型命名跟着标准库走：类型 `PascalCase`，函数/变量 `snake_case`，常量 `kCamel` 或 `kSnake`

### 规则三：边界只交换两种东西

进去的是**类型化参数**，出来的是 `Result<T>`。
`core` 从不产出给用户看的句子——它返回机器可读的失败标识（如 `task.title.empty`），
中文文案由 `app/presenter.cpp` 一处负责。换语言、换图标只改那一个文件。

### 规则四：错误处理策略第一天就定死

- **可预期的失败**（用户没填标题、找不到记录）→ 返回值 `Result<T>`
- **不可恢复的失败**（构造不出对象、文件打不开）→ 抛异常

另有一条硬约束：**异常绝对不许越过 C 接口边界**。webview / Qt 的绑定回调都是 C 接口，
从里面抛异常是未定义行为，所以那一层一律用不抛异常的写法
（例如 `json::parse(args, nullptr, false)`）。

## 线上协议

页面加载后，webview 把几个函数注入到 JS 的 `window` 上，调用后返回 Promise。
模板提供三个：

```js
window.getState()        // 查询状态
window.addTask(title)    // 添一条记录
window.clearTasks()      // 清空
```

返回的 JSON 信封（字段名就是前后端契约）：

```json
{
  "ok": false,
  "level": "error",
  "event": "failed",
  "status": "❌ 标题不能为空",
  "detail": "标题不能为空",
  "error": "task.title.empty",
  "messages": ["写测试"]
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
| `messages` | 列表内容（已格式化） |

界面只认 `ok` 和 `level`，**不去嗅探 ✅❌ 这类文本前缀**——
文案可以在后端随便改，界面不受影响。

## 加业务功能的固定动作

以"再加一个优先级字段"或"新增一个订单模块"为例：

1. `core/include/myapp/<业务>.h` —— 数据结构 + 业务类声明
2. `core/src/<业务>.cpp` —— 业务规则，失败返回 `Result` + `Error{code, key}`
3. `core/CMakeLists.txt` 里把新 `.cpp` 加进 `myapp_core`
4. `tests/<业务>_test.cpp` —— **先测业务，再接界面**
5. `app/presenter.cpp` 的 `lookup` 里给新 key 加中文（多数情况复用已有错误码，不用改）
6. `app/controller.cpp` 里注册一行 `bind`
7. 需要的话，在 `app/web/index.html` 加控件

第 5、6 步通常只有几行。如果发现某次改动不得不动架构代码，
先想一想是不是业务边界划错了。

## 目录什么时候该长

目录随需要增长，**不提前建空目录**（空目录是负债，它承诺了一个还没来的未来）。

| 触发条件 | 加什么 |
|---|---|
| 出现第二种界面（比如同时要 Qt 版或命令行版） | `app/<壳名>/` 拆目录，`presenter`/`protocol` 提为共用 |
| `core` 里出现第二个业务域 | `core/include/myapp/<域>/` 分子目录 |
| 存储要换实现（内存 → SQLite） | 抽 `<域>_repository.h` 接口，实现放 `app/` |
| 出现第二套传输（REST/CLI） | 抽 `presenter`/`protocol` 的公共部分 |
| 第三方依赖 ≥ 3 个 | 每个依赖一个子目录，或改用 `find_package` |

配一条纪律：**三则抽**——同一种模式出现第三次才抽象。

## 这套约定是抄谁的

每个决定都有出处，不是自创：

| 出处 | 取哪一条 |
|---|---|
| C++ Core Guidelines（Stroustrup / Sutter） | E.1 错误策略先定、E.2/E.3 异常用法、R.1/R.11 RAII、I.1 接口显式 |
| Google C++ Style Guide | 命名规范、include 从项目根写全、include 顺序 |
| A Philosophy of Software Design（Ousterhout） | 深模块、信息隐藏、注释写"为什么" |
| Clean Architecture | 只取"依赖指向内"这一条（不取它的四层全套） |
| hattonl/cpp-project-structure | 目录命名、`include/<项目名>/` 公开头文件分离（不取它的 CMake 写法） |

另外提醒一句：`Result<T>` 不是自创，C++23 已把它标准化为 `std::expected<T, E>`。
等编译器支持了，把 `result.h` 换掉即可，业务代码不用改。

## 依赖

- CMake ≥ 3.16，支持 C++17 的编译器
- GTK 3 + WebKitGTK 4.1（`pkg-config` 能找到 `gtk+-3.0` 与 `webkit2gtk-4.1`）
- 第三方单头文件已内置：`third_party/webview/webview.h`、`third_party/nlohmann/json.hpp`

## 构建、运行、测试

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/bin/myapp             # 需要图形环境
cd build && ctest --output-on-failure   # 不需要图形环境
```

## 若干说明

- **HTML 在构建期内嵌**：CMake 把 `app/web/index.html` 转成 `index_html.h`，
  运行时不需要额外文件，改完 HTML 要重新构建。想边改边看，把 `main.cpp` 里的
  `window.set_html(kIndexHtml)` 换成
  `window.navigate("file:///绝对路径/app/web/index.html")`。
- **`index.html` 里不要写 CMake 的 `"${...}"` 形式**，否则会在配置阶段被展开。
- **回包用 `ordered_json`**：默认的 `json` 底层是 `std::map`，`dump()` 会按 key 字母序输出；
  `ordered_json` 保持写入顺序，读日志更直观。
- **第三方单头文件**：`nlohmann/json` 被包成 `nlohmann_json::nlohmann_json` 目标，
  以后想换 `find_package` 系统包时，链接处不用改。
