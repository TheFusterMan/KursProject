#pragma once

#include <QDialog>

// ѕр€мые объ€влени€ дл€ уменьшени€ количества #include в .h файлах
class QLineEdit;
class QDialogButtonBox;

class DeleteClientDialog : public QDialog
{
    Q_OBJECT

public:
    // явный конструктор, принимающий родительский виджет
    explicit DeleteClientDialog(QWidget* parent = nullptr);

    // ћетоды дл€ получени€ введенных пользователем данных
    QString getINN() const;
    QString getFIO() const;
    QString getPhone() const;

private:
    // ”казатели на виджеты ввода
    QLineEdit* innEdit;
    QLineEdit* nameEdit;
    QLineEdit* surnameEdit;
    QLineEdit* patronymicEdit;
    QLineEdit* phoneEdit;

    // ”казатель на блок кнопок (OK/Cancel)
    QDialogButtonBox* buttonBox;
};