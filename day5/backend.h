#pragma once
#include <QObject>
#include <QString>
#include <QVariantList>

class Backend : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString statusText READ statusText WRITE setStatusText NOTIFY statusTextChanged)

    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)

  public:
    explicit Backend(QObject *parent = nullptr);

    QString statusText() const;
    void setStatusText(const QString &text);

    QVariantList messages() const;

    Q_INVOKABLE void submit(const QString &name, int age);
    Q_INVOKABLE void clear();

  signals:
    void statusTextChanged();
    void messagesChanged();
    void submitSucceeded(const QString &summary);
    void submitFailed(const QString &reason);

  private:
    QString m_statusText = "等待输入...";
    QVariantList m_messages;

    bool validate(const QString &name, int age, QString &error);
    void store(const QString &summary);
};