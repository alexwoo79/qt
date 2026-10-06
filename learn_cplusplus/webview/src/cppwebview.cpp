// =====================================================================
//  webview 测试项目
// =====================================================================
//  库       : webview 0.12.0  (https://github.com/webview/webview)
//  后端     : GTK 4 + WebKitGTK 6.0（已安装到 /usr/local）
//
//  构建: 在 youtube/ 目录下执行  make
//  运行: ./myapp
//
//  页面仍然是独立的 src/index.html，但不会在运行时去读它：
//  make 会先把它转成 obj/index_html.h（一个原始字符串字面量），
//  再和下面的代码一起编译，于是页面内容就"长"在可执行文件里了。
//  好处是发出去只有一个文件，不怕把 html 忘在别处；
//  代价是改完 html 要重新 make 才生效。
//
//  网页里的按钮通过绑定（binding）调用下面两个 C++ 函数：
//      app_version()   -> 返回库版本号
//      greet(name)     -> 返回一句 C++ 拼出来的问候语
// =====================================================================

#include <webview/webview.h>

#include "index_html.h" // 由 make 从 src/index.html 生成

#include <iostream>
#include <string>

namespace {

// JS 里调用 greet("世界")，C++ 收到的是参数数组的 JSON 文本：["世界"]
// 这里只取第一个元素并把外层引号剥掉，够用就好；
// 参数一多就该换成正经的 JSON 库（例如 nlohmann/json）。
std::string first_arg(std::string args) {
  if (args.size() >= 2 && args.front() == '[' && args.back() == ']') {
    args = args.substr(1, args.size() - 2);
  }
  if (args.size() >= 2 && args.front() == '"' && args.back() == '"') {
    args = args.substr(1, args.size() - 2);
  }
  return args;
}

// 返回值必须是合法 JSON —— JS 那边会对它做 JSON.parse，
// 所以返回一段普通文本时得自己加上引号（顺手把引号和反斜杠转义掉）。
std::string to_json(const std::string &text) {
  std::string out = "\"";
  for (const char c : text) {
    if (c == '"' || c == '\\') {
      out += '\\';
    }
    out += c;
  }
  return out + '"';
}

} // namespace

int main() {
  try {
    webview::webview w(false, nullptr); // 第一个参数 true = 调试模式
    w.set_title("webview 测试项目");
    w.set_size(560, 400, WEBVIEW_HINT_NONE);

    // 没有参数：返回库版本号
    w.bind("app_version", [](const std::string &) -> std::string {
      return to_json(WEBVIEW_VERSION_NUMBER);
    });

    // 一个字符串参数：返回一句问候
    w.bind("greet", [](const std::string &req) -> std::string {
      const std::string name = first_arg(req);
      std::cout << "[C++] greet(" << name << ")" << std::endl;
      return to_json("你好，" + name + "！这句是 C++ 拼出来的（webview " +
                     WEBVIEW_VERSION_NUMBER + "）");
    });

    w.set_html(kIndexHtml);
    std::cout << "页面已编译进程序，关闭窗口即可退出。" << std::endl;
    w.run();
  } catch (const webview::exception &e) {
    std::cerr << "运行出错：" << e.what() << std::endl;
    return 1;
  }

  return 0;
}
