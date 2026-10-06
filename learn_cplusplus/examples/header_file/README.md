# HeaderFileExample

这个示例演示如何用头文件声明函数，并在多个源文件中组织实现。

## 环境要求

- CMake 3.10 或更高版本
- C++ 编译器，例如 Apple Clang

## 配置和构建

在本目录打开终端，执行：

```sh
cmake -S . -B build
cmake --build build
```

`-S .` 指定当前目录为源代码目录，`-B build` 将生成的构建文件放在 `build` 目录。CMake 会读取 `CMakeLists.txt`，并将 `main.cpp` 和 `log.cpp` 编译、链接为 `HeaderFileExample`。

## 运行

在 macOS 或 Linux 上执行：

```sh
./build/HeaderFileExample
```

预期输出：

```text
Initializing log system...
Hello, World!
```

修改源文件后，重新执行 `cmake --build build` 即可增量构建。


