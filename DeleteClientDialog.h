#pragma once

#include <QDialog>

// ������ ���������� ��� ���������� ���������� #include � .h ������
class QLineEdit;
class QDialogButtonBox;

class DeleteClientDialog : public QDialog
{
    Q_OBJECT

public:
    // ����� �����������, ����������� ������������ ������
    explicit DeleteClientDialog(QWidget* parent = nullptr);

    // ������ ��� ��������� ��������� ������������� ������
    QString getINN() const;
    QString getFIO() const;
    QString getPhone() const;

private:
    // ��������� �� ������� �����
    QLineEdit* innEdit;
    QLineEdit* nameEdit;
    QLineEdit* surnameEdit;
    QLineEdit* patronymicEdit;
    QLineEdit* phoneEdit;

    // ��������� �� ���� ������ (OK/Cancel)
    QDialogButtonBox* buttonBox;
};