#pragma once
#ifndef ADDCLIENTDIALOG
#define ADDCLIENTDIALOG

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class AddClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddClientDialog(QWidget* parent = nullptr);

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