#pragma once
#ifndef ADDCONSULTATIONDIALOG
#define ADDCONSULTATIONDIALOG

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class AddConsultationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddConsultationDialog(QWidget* parent = nullptr);

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