// counter.h
#pragma once
#include <QObject>

class Counter : public QObject {
  Q_OBJECT // ← 必须有，且必须在类定义的开头

      public :

      explicit Counter(QObject *parent = nullptr);

    // 普通方法（QML 暂时还看不到，Day 4 会变成 Q_INVOKABLE）
    void increment();
    void reset();

    int value() const; // 访问器

  signals:
    // 信号：只有声明，没有实现
    // moc 会自动生成实现
    void valueChanged(int newValue);
    void resetDone();

  private:
    int m_value = 0;
};