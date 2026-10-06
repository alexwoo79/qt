#include "backend.h"

Backend::Backend(QObject *parent) : QObject(parent) {
}

QString Backend::statusText() const {
    return m_statusText;
}

void Backend::setStatusText(const QString &text) {
    if (m_statusText == text)
        return;
    m_statusText = text;
    emit statusTextChanged();
}

QVariantList Backend::messages() const {
    return m_messages;
}

void Backend::submit(const QString &name, int age) {
    QString error;
    if (!validate(name, age, error)) {
        setStatusText("❌ " + error);
        emit submitFailed(error);
        return;
    }

    QString summary = QString("%1 (%2 岁)").arg(name).arg(age);
    store(summary);

    setStatusText("✅ 已添加: " + summary);
    emit submitSucceeded(summary);
}

void Backend::clear() {
    m_messages.clear();
    emit messagesChanged();
    setStatusText("已清空");
}

bool Backend::validate(const QString &name, int age, QString &error) {
    if (name.trimmed().isEmpty()) {
        error = "姓名不能为空";
        return false;
    }
    if (age < 1 || age > 150) {
        error = "年龄必须在 1-150 之间";
        return false;
    }
    return true;
}

void Backend::store(const QString &summary) {
    m_messages.append(summary);
    emit messagesChanged();
}