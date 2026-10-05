#pragma once

#include <QObject>
#include <QVariantList>

class Counter : public QObject {
    Q_OBJECT

    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged FINAL)
    // value 改变时也通过同一信号通知 QML 更新 doubled。
    Q_PROPERTY(int doubled READ doubled NOTIFY valueChanged)
    Q_PROPERTY(int step READ step WRITE setStep NOTIFY stepChanged)
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)

  public:
    explicit Counter(QObject *parent = nullptr);

    int value() const;
    int doubled() const;
    int step() const;
    QVariantList history() const;
    void setValue(int v);
    void setStep(int step);
    Q_INVOKABLE void increment();
    Q_INVOKABLE void reset();

  signals:
    void valueChanged(int newValue);
    void stepChanged(int newStep);
    void historyChanged();

  private:
    int m_value = 0;
    int m_step = 1;
    QVariantList m_history;
};