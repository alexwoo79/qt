#include "counter.h"

Counter::Counter(QObject *parent) : QObject(parent) {
}

int Counter::value() const {
    return m_value;
}

int Counter::doubled() const {
    return m_value * 2;
}

int Counter::step() const {
    return m_step;
}

QVariantList Counter::history() const {
    return m_history;
}

void Counter::setValue(int v) {
    const bool changed = m_value != v;
    m_value = v;
    m_history.append(m_value);
    if (changed) {
        emit valueChanged(m_value); // 练习：注释后，QML 属性绑定不会收到 value 的变化通知。
    }
    emit historyChanged();
}

void Counter::setStep(int step) {
    if (m_step == step) {
        return;
    }
    m_step = step;
    emit stepChanged(m_step);
}

void Counter::increment() {
    setValue(m_value + m_step);
}

void Counter::reset() {
    setValue(0);
}