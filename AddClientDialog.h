#pragma once
#ifndef ADDCLIENTDIALOG
#define ADDCLIENTDIALOG

#include <QDialog>

// Предварительное объявление классов, чтобы не включать лишние заголовки
class QLineEdit;
class QDialogButtonBox;

class AddClientDialog : public QDialog
{
    Q_OBJECT // Обязательный макрос для классов с сигналами и слотами

public:
    // Конструктор
    explicit AddClientDialog(QWidget* parent = nullptr);

    // Методы для получения введенных данных
    QString getINN() const;
    QString getFIO() const;
    QString getPhone() const;

private:
    QLineEdit* innEdit;
    QLineEdit* nameEdit;
    QLineEdit* surnameEdit;
    QLineEdit* patronymicEdit;
    QLineEdit* phoneEdit;
    QDialogButtonBox* buttonBox;
};

#endif // ADDCLIENTDIALOG