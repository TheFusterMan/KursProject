#pragma once

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class DeleteConsultationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteConsultationDialog(QWidget* parent = nullptr);

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