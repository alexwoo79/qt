// 组装根：只负责把 core（业务）和 app（界面）接起来，不写业务、不写文案。
#include "controller.h"
#include "presenter.h"

#include "ledger/ledger.h"

#include <webview/webview.h>

#include "index_html.h" // 由 CMake 从 web/index.html 生成

#include <iostream>

int main() {
    try {
        // 第一个参数 true 会打开开发者工具；第二个参数传 nullptr 表示自己建窗口。
        webview::webview window(false, nullptr);
        window.set_title("记账本");
        window.set_size(420, 620, WEBVIEW_HINT_NONE);

        ledger::LedgerService service;                  // 业务：数据与规则
        ledger::Presenter presenter;                    // 展示：所有中文文案
        ledger::Controller controller(window, service, presenter); // 控制器：翻译
        controller.bind();

        // 页面在编译期内嵌进可执行文件。想边改边看，换成：
        //   window.navigate("file:///绝对路径/app/web/index.html");
        window.set_html(kIndexHtml);
        window.run();
    } catch (const webview::exception &e) {
        std::cerr << "webview 启动失败: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
