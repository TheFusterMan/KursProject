#pragma once

#include <QObject>
#include <QString>

// Используем шаблон Singleton, чтобы иметь единственную точку для логирования
class DebugLogger : public QObject
{
    Q_OBJECT

private:
    // Конструктор приватный, чтобы нельзя было создать экземпляр извне
    explicit DebugLogger(QObject* parent = nullptr);

public:
    // Удаляем конструктор копирования и оператор присваивания
    DebugLogger(const DebugLogger&) = delete;
    DebugLogger& operator=(const DebugLogger&) = delete;

    // Статический метод для доступа к единственному экземпляру
    static DebugLogger& instance();

    // Статический метод для отправки сообщения в лог
    static void log(const QString& message);

signals:
    // Сигнал, который будет испускаться при поступлении нового сообщения
    void messageLogged(const QString& message);
};