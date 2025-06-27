#include "DebugLogger.h"
#include <QDateTime> // Для добавления временных меток

DebugLogger::DebugLogger(QObject* parent) : QObject(parent) {}

// Реализация статического метода доступа
DebugLogger& DebugLogger::instance()
{
    static DebugLogger loggerInstance; // Создается один раз при первом вызове
    return loggerInstance;
}

// Реализация статического метода для логирования
void DebugLogger::log(const QString& message)
{
    // Добавляем временную метку для наглядности
    QString formattedMessage = QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + " -> " + message;

    // Получаем экземпляр и испускаем от его имени сигнал
    // Это безопасно для вызова из любого потока (хотя в нашем случае все в одном)
    emit instance().messageLogged(formattedMessage);
}