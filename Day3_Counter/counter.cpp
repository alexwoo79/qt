// counter.cpp
#include "counter.h"
#include <QDebug>

Counter::Counter(QObject *parent) : QObject(parent) {
    // 构造函数：把 parent 传给 QObject
}

void Counter::increment() {
    m_value++;
    emit valueChanged(m_value); // ← 发射信号
}

void Counter::reset() {
    m_value = 0;
    emit resetDone();
}

int Counter::value() const {
    return m_value;
}