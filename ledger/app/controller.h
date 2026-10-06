#pragma once

#include "presenter.h"

#include "ledger/ledger.h"

#include <webview/webview.h>

#include <string>

namespace ledger {

// 控制器：把界面上的动作翻译成业务调用，再把业务结果翻译回界面能用的数据。
// 它认识 webview 和 JSON，但自己不含任何业务规则。
//
// 新增业务动作 = 加一个 handleXxx + 一行 bind，其余不用动。
class Controller {
public:
    Controller(webview::webview &window, LedgerService &service, Presenter presenter);

    // 把三个动作注册到页面上：window.getState / addEntry / clearEntries
    void bind();

private:
    std::string handleState();
    std::string handleAdd(const std::string &argsJson);
    std::string handleClear();

    // 组装一次回包：状态文本用当前 m_statusText，列表和合计统一走 Presenter。
    std::string reply(bool ok, const char *level, std::string event, std::string detail,
                      std::string errorKey);

    webview::webview &m_window;
    LedgerService &m_service;
    Presenter m_presenter;
    std::string m_statusText;
};

} // namespace ledger
