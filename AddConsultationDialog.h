#pragma once
#ifndef ADDCONSULTATIONDIALOG
#define ADDCONSULTATIONDIALOG

#include <QDialog>

// Предварительное объявление классов, чтобы не включать лишние заголовки
class QLineEdit;
class QDialogButtonBox;

class AddConsultationDialog : public QDialog
{
    Q_OBJECT // Обязательный макрос для классов с сигналами и слотами

public:
    // Конструктор
    explicit AddConsultationDialog(QWidget* parent = nullptr);

    // Методы для получения введенных данных
    QString getINN() const;
    QString getFIO() const;
    QString getDate() const;
    QString getTheame() const;

private:
    QLineEdit* innEdit;
    QLineEdit* theameEdit;
    QLineEdit* nameEdit;
    QLineEdit* surnameEdit;
    QLineEdit* patronymicEdit;
    QLineEdit* dateEdit;
    QDialogButtonBox* buttonBox;
};

#endif // ADDCONSULTATIONDIALOG