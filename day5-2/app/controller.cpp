#include "controller.h"

#include "protocol.h"

#include <iostream>
#include <utility>

namespace day5_2 {

Controller::Controller(webview::webview &window, PersonService &service, Presenter presenter)
    : m_window(window), m_service(service), m_presenter(std::move(presenter)),
      m_statusText(m_presenter.idleStatus()) {}

void Controller::bind() {
    m_window.bind("getState",
                  [this](const std::string &) -> std::string { return handleState(); });

    m_window.bind("submitEntry", [this](const std::string &args) -> std::string {
        return handleSubmit(args);
    });

    m_window.bind("clearMessages",
                  [this](const std::string &) -> std::string { return handleClear(); });
}

std::string Controller::handleState() {
    m_statusText = m_presenter.idleStatus();
    return reply(true, "info", "state", "", "");
}

std::string Controller::handleSubmit(const std::string &argsJson) {
    SubmitArgs args;
    if (!decodeSubmitArgs(argsJson, args)) {
        const Error error{ErrorCode::invalid_input, "protocol.bad_args", {}};
        m_statusText = m_presenter.failedStatus(error);
        const std::string payload = reply(false, "error", "failed", m_presenter.messageFor(error),
                                          error.key);
        std::cout << "C++ 通知失败: " << m_statusText << '\n';
        return payload;
    }

    const Result<Person> result = m_service.submit(args.name, args.age);

    std::string payload;
    if (result.ok()) {
        m_statusText = m_presenter.submittedStatus(result.value);
        payload = reply(true, "ok", "submitted", m_presenter.formatPerson(result.value), "");
        std::cout << "C++ 通知成功: " << m_statusText << '\n';
    } else {
        m_statusText = m_presenter.failedStatus(result.error);
        payload = reply(false, "error", "failed", m_presenter.messageFor(result.error),
                        result.error.key);
        std::cout << "C++ 通知失败: " << m_statusText << '\n';
    }

    // C++ -> JS：用 dispatch 回到 UI 线程，再主动把这次操作的事件推给页面。
    m_window.dispatch([this, payload]() {
        m_window.eval("window.__onBackendEvent && window.__onBackendEvent(" + payload + ")");
    });

    return payload;
}

std::string Controller::handleClear() {
    m_service.clear();
    m_statusText = m_presenter.clearedStatus();
    return reply(true, "info", "cleared", "", "");
}

std::string Controller::reply(bool ok, const char *level, std::string event, std::string detail,
                              std::string errorKey) {
    Envelope envelope;
    envelope.ok = ok;
    envelope.level = level;
    envelope.event = std::move(event);
    envelope.status = m_statusText;
    envelope.detail = std::move(detail);
    envelope.error = std::move(errorKey);
    envelope.messages = m_presenter.formatPeople(m_service.people());
    return encode(envelope);
}

} // namespace day5_2
