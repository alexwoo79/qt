// main.cpp
#include "counter.h"
#include <QCoreApplication>
#include <QDebug>

void onValueChanged(int value) {
    qDebug() << "值变了:" << value;
}

void onResetDone() {
    qDebug() << "已重置";
}

int main(int argc, char *argv[]) {

    // 定义 QCoreApplication 对象，管理应用程序的控制流和主要设置
    QCoreApplication app(argc, argv);

    Counter counter;

    // 把 valueChanged 信号连接到普通函数
    QObject::connect(&counter, &Counter::valueChanged, &app, onValueChanged);

    // 把 resetDone 信号连接到普通函数
    QObject::connect(&counter, &Counter::resetDone, &app, onResetDone);

    qDebug() << "开始测试";
    counter.increment(); // 触发 valueChanged(1)
    counter.increment(); // 触发 valueChanged(2)
    counter.increment(); // 触发 valueChanged(3)

    counter.reset(); // 触发 resetDone

    qDebug() << "结束测试";
    return 0;
}