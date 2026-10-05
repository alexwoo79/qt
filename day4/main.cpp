#include "counter.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // 保证计数器在 QML 引擎使用期间一直有效。
    Counter counter;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("counter", &counter);

    engine.loadFromModule("day4", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}