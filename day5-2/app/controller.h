#pragma once

#include "presenter.h"

#include "day5_2/person.h"

#include <webview/webview.h>

#include <string>

namespace day5_2 {

// 控制器：把界面上的动作翻译成业务调用，再把业务结果翻译回界面能用的数据。
// 它认识 webview 和 JSON，但自己不含任何业务规则。
class Controller {
public:
    Controller(webview::webview &window, PersonService &service, Presenter presenter);

    // 把三个动作注册到页面上：window.getState / submitEntry / clearMessages
    void bind();

private:
    std::string handleState();
    std::string handleSubmit(const std::string &argsJson);
    std::string handleClear();

    // 组装一次回包：状态文本用当前 m_statusText，列表统一走 Presenter。
    std::string reply(bool ok, const char *level, std::string event, std::string detail,
                      std::string errorKey);

    webview::webview &m_window;
    PersonService &m_service;
    Presenter m_presenter;
    std::string m_statusText;
};

} // namespace day5_2
