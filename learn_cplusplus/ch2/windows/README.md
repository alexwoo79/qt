# Windows 版：QML 计算器（交叉编译产物）

`calculator_qml_win64/` 是把 `../calculator_qml.cpp` + `../Main.qml` 交叉编译后得到的
Windows x86_64 可执行程序，整个文件夹拷到 Windows 上就能直接运行，不需要装 Qt。

![Windows 上运行效果](windows-build-running.png)

## 在 Windows 上运行

1. 把 **整个** `calculator_qml_win64` 文件夹复制到 Windows 机器上（保持目录结构不变）。
2. 双击 `calculator_qml_win64\calculator_qml.exe`。

要求：64 位 Windows 10 / 11。无需安装器、无需 Qt 运行库、无需管理员权限。

## 文件夹里有什么

| 内容 | 大小 | 说明 |
| --- | --- | --- |
| `calculator_qml.exe` | 602 KB | 主程序（Windows GUI 子系统，双击不弹控制台） |
| `Qt6*.dll` | 45 MB | Qt 运行库，按 PE 导入表闭包精确挑选，只留实际用到的 20 个 |
| `qml/` | 2.2 MB | 只含 `Main.qml` 真正导入的 QML 模块 |
| `plugins/platforms/` | 1.3 MB | 只保留必需的 `qwindows.dll` |
| `libgcc_s_seh-1.dll`、`libstdc++-6.dll`、`libwinpthread-1.dll` | 2.6 MB | MinGW C++ 运行库 |

合计 **52 MB / 140 个文件**（优化前是 100 MB / 1526 个文件）。

## 两种交付形式

| 形式 | 文件 | 特点 |
| --- | --- | --- |
| 绿色版 | `calculator_qml_win64/`（52 MB） | 拷到 Windows 上双击即用，不写注册表 |
| 安装包 | `calculator_qml-setup-1.0.0.exe`（14 MB） | 正常安装/卸载流程，快捷方式、卸载入口齐全 |

### 安装包（NSIS）

* 界面语言：简体中文（固定中文，不跟随系统语言；同时保留繁体/英文会影响默认语言选择，故只编译中文）
* 安装位置：`C:\Program Files\CalculatorQml`，安装过程需要管理员权限
* 快捷方式：开始菜单、桌面（安装时可勾选）
* 卸载：设置 → 应用 里的「简单计算器」，或安装目录下的 `uninstall.exe`
* 覆盖安装：检测到旧版本会先静默卸载旧版再装新版
* 只允许 64 位 Windows（`.onInit` 里检查 `RunningX64`）
* 压缩方式：LZMA solid，101 MB → 23 MB

## 构建方式

```
./build-windows.sh
```

脚本做的事情：

1. 首次运行时从镜像下载官方 **Qt 6.11.2 for Windows (MinGW)** 到 `~/.cache/qt-win-sdk`
   （可用 `QT_WIN_SDK=/path/to/qt` 指定已有目录，`QT_MIRROR=...` 换镜像）。
2. 用 `cmake` + `toolchain-mingw64.cmake` 交叉编译：

   * 交叉编译器：`x86_64-w64-mingw32-g++`（Arch 的 MinGW-w64 GCC 16.2）
   * 目标库：Windows 版 Qt（`QT_WIN_ROOT`）
   * 宿主工具：Linux 原生 Qt 6.11.2（`QT_HOST_PATH=/usr`），提供 `moc`、`rcc`、
     `qmlcachegen`、`qmltyperegistrar`、`qmlimportscanner`

3. 组装可移植目录：按 PE 导入表计算 Qt DLL 依赖闭包，带上 exe 真正用到的 QML 模块。
4. 校验：所有非系统 DLL 依赖都能在目录内解析，每个 QML 模块的插件都在。

依赖（Arch / Omarchy）：

```
sudo pacman -S mingw-w64-gcc cmake ninja curl libarchive qt6-base qt6-declarative
```

生成安装包：

```
./build-installer.sh
```

NSIS 需要 `makensis`。Arch 官方仓库里没有，从 AUR 装：`yay -S nsis`。
脚本优先使用系统里的 `makensis`；找不到时会从 Ubuntu 源下载 `nsis` + `nsis-common`
两个包解压到 `~/.cache/nsis` 使用，不需要 root。

## 体积优化（这一版做了什么）

| 项目 | 优化前 | 优化后 |
| --- | --- | --- |
| 绿色版目录 | 100 MB / 1526 个文件 | **52 MB / 140 个文件** |
| 安装包 | 23 MB | **14 MB** |
| 部署的 QML 模块 | 16 MB、26 个（含 6 套控件样式） | **2.2 MB、14 个（只有 Basic 一套）** |
| 平台插件 | qwindows + qdirect2d + qminimal + qoffscreen | 只留 qwindows |
| 图片格式插件 | gif / ico / jpeg | 全部去掉（界面不用位图） |
| 软件渲染回退 | opengl32sw 20 MB + d3dcompiler_47 | 都不带（Win10+ 自带 d3dcompiler_47） |
| QML 提前编译 | 43 个绑定里 21 个编译失败 | **39 个里 36 个编译成功** |

**QML（`Main.qml`）**

* `import QtQuick.Controls` → `import QtQuick.Controls.Basic`：显式指定控件样式，
  部署时就不必带上 Fusion / Material / Universal / Imagine / Windows 五套样式（省约 10 MB）
* 委托里 `parent.text`、`parent.highlighted` 改成用具名 id（`operationButton.*`）——
  用 `parent` 时编译器推不出类型，绑定只能留到运行时解释
* 按钮模型从 JS 数组改成 `ListModel` + `required property string label/value`，角色类型明确
* 状态从 `ApplicationWindow` 的属性挪进 `QtObject { id: calcState }`；
  **id 不能叫 `state`**，那是每个 Item 自带的属性，会被遮蔽
* 一元运算符判断由数组 `indexOf` 改成逐项 `===` 比较（数组是 `var` + 未类型化 JS 调用，无法提前编译）

**构建（`../CMakeLists.txt`）**

* 打开 `QT_DISCARD_FILE_CONTENTS`：QML 源码不再嵌入 exe（实测 exe 里已搜不到 `import QtQuick`）
* `-Os -ffunction-sections -fdata-sections -Wl,--gc-sections`，并默认 Release

**部署（`build-windows.sh`）**

* QML 模块不再整棵 `qml/` 拷过去，而是读 `qmlimportscanner` 的扫描结果按模块精确复制
* 样式目录再过一层过滤：只保留 `Main.qml` 里 import 的那套样式
* 平台插件只留 `qwindows.dll`；不部署 `imageformats/`、`opengl32sw.dll`、`d3dcompiler_47.dll`

需要软件渲染回退的机器（老显卡 / 没有显卡驱动的虚拟机）：

```
WITH_SOFTWARE_RENDERER=1 ./build-windows.sh
```

> exe 从 400 KB 变成 602 KB 是正常的：编译好的绑定代码进了二进制（换来启动时不用再解释 QML），
> 同时 QML 源码被剔除掉了。

## 两个值得知道的细节

**1. 用链接器直接指定 Windows 子系统**

常规做法是给目标设 `WIN32_EXECUTABLE`，但 Qt 预编译的
`libQt6EntryPoint.a` 是按 MSVCRT 运行时编译的，而 Arch 的 MinGW-w64 工具链默认走
UCRT，链接时会报 `undefined reference to '__imp___argc'`。因此这里不用
Qt 的入口点，改成：

```
-DCMAKE_EXE_LINKER_FLAGS="-Wl,--subsystem,windows -Wl,-e,mainCRTStartup"
```

即保留 `main()` / `mainCRTStartup`，只把子系统标成 GUI —— 双击不出现黑框控制台。
调试时想去掉这个标志，就能拿回控制台输出。

**2. 运行时 DLL 的版本**

Qt 的 Windows 包是 MinGW-w64 GCC 13.1 + MSVCRT 编译的，本机交叉工具链是
GCC 16 + UCRT。程序里带的是**本机工具的** `libstdc++-6.dll` / `libgcc_s_seh-1.dll` /
`libwinpthread-1.dll`：GCC 的 C++ 运行库向下兼容，新版能提供 Qt DLL 需要的所有符号，
反过来则不成立，所以不能换成 Qt 包里那份更旧的运行库。

## 验证情况

* 编译、部署、依赖闭包校验全部通过。
* 在 Wine 11.18 下实际运行：窗口标题「简单计算器」正常打开，界面渲染正确（见上图）。
* 功能实测：用一份会自动点击的测试版本跑通「点击 tan 按钮 → 计算」，
  结果显示 `0.546302489844`（= tan 0.5，正确），同时「第二个数」输入框按一元运算隐藏——
  说明委托里的点击处理、状态、C++ 计算、结果回填整条链路都正常。
* 安装包在 Wine 下完整验证：

  * 静默安装 → `C:\Program Files\CalculatorQml` 落地 141 个文件 / 52 MB；
    注册表卸载项（显示名、版本、图标、估算大小）正确写入，中文显示名以 UTF-16 存储无误
  * 启动已安装的副本 → 正常打开
  * 静默卸载 → 安装目录、快捷方式、注册表项全部清除

* 尚未在真实 Windows 物理机上运行过。另外 Wine 里中文会显示成方框，那是这台
  Linux 的 Wine 环境没有可用的中文字体所致（窗口标题这类由合成器绘制的文字是正常的），
  Windows 自带中文字体与字体链接，不受影响。

## 重新构建（增量）

构建目录在 `windows/.build-win64/`，改完 `calculator_qml.cpp` 或 `Main.qml` 后
重新执行 `./build-windows.sh` 即可，几秒钟完成。
