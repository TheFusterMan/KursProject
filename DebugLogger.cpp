#include "DebugLogger.h"
#include <QDateTime> // ��� ���������� ��������� �����

DebugLogger::DebugLogger(QObject* parent) : QObject(parent) {}

// ���������� ������������ ������ �������
DebugLogger& DebugLogger::instance()
{
    static DebugLogger loggerInstance; // ��������� ���� ��� ��� ������ ������
    return loggerInstance;
}

// ���������� ������������ ������ ��� �����������
void DebugLogger::log(const QString& message)
{
    // ��������� ��������� ����� ��� �����������
    QString formattedMessage = QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + " -> " + message;

    // �������� ��������� � ��������� �� ��� ����� ������
    // ��� ��������� ��� ������ �� ������ ������ (���� � ����� ������ ��� � �����)
    emit instance().messageLogged(formattedMessage);
}