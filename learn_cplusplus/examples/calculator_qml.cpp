#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>
#include <QString>
#include <QVariantMap>

#include <cmath>

class Calculator : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    Q_INVOKABLE QVariantMap calculate(const QString& operation, double first, double second) const {
        double result = 0.0;

        if (operation == QStringLiteral("sin")) {
            result = std::sin(first);
        } else if (operation == QStringLiteral("cos")) {
            result = std::cos(first);
        } else if (operation == QStringLiteral("tan")) {
            result = std::tan(first);
        } else if (operation == QStringLiteral("log")) {
            if (first <= 0.0) {
                return error(QStringLiteral("自然对数的参数必须大于 0。"));
            }
            result = std::log(first);
        } else if (operation == QStringLiteral("exp")) {
            result = std::exp(first);
        } else if (operation == QStringLiteral("+")) {
            result = first + second;
        } else if (operation == QStringLiteral("-")) {
            result = first - second;
        } else if (operation == QStringLiteral("*")) {
            result = first * second;
        } else if (operation == QStringLiteral("/")) {
            if (second == 0.0) {
                return error(QStringLiteral("除数不能为 0。"));
            }
            result = first / second;
        } else if (operation == QStringLiteral("%")) {
            if (second == 0.0) {
                return error(QStringLiteral("取余运算的除数不能为 0。"));
            }
            result = std::fmod(first, second);
        } else if (operation == QStringLiteral("^")) {
            result = std::pow(first, second);
        } else {
            return error(QStringLiteral("不支持这个运算符。"));
        }

        if (!std::isfinite(result)) {
            return error(QStringLiteral("结果超出有效范围。"));
        }

        return {{QStringLiteral("ok"), true}, {QStringLiteral("value"), result}};
    }

private:
    static QVariantMap error(const QString& message) {
        return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), message}};
    }
};

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    Calculator calculator;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("calculator"), &calculator);
    engine.loadFromModule(QStringLiteral("LearnCpp.Calculator"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

#include "calculator_qml.moc"
