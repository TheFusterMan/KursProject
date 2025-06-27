#pragma once

#include <QObject>
#include <QString>

// ���������� ������ Singleton, ����� ����� ������������ ����� ��� �����������
class DebugLogger : public QObject
{
    Q_OBJECT

private:
    // ����������� ���������, ����� ������ ���� ������� ��������� �����
    explicit DebugLogger(QObject* parent = nullptr);

public:
    // ������� ����������� ����������� � �������� ������������
    DebugLogger(const DebugLogger&) = delete;
    DebugLogger& operator=(const DebugLogger&) = delete;

    // ����������� ����� ��� ������� � ������������� ����������
    static DebugLogger& instance();

    // ����������� ����� ��� �������� ��������� � ���
    static void log(const QString& message);

signals:
    // ������, ������� ����� ����������� ��� ����������� ������ ���������
    void messageLogged(const QString& message);
};