#include "AddConsultationDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

AddConsultationDialog::AddConsultationDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Добавить консультацию");

    innEdit = new QLineEdit(this);
    nameEdit = new QLineEdit(this);
    surnameEdit = new QLineEdit(this);
    patronymicEdit = new QLineEdit(this);
    theameEdit = new QLineEdit(this);
    dateEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddConsultationDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddConsultationDialog::reject);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"Фамилия юриста:", nameEdit);
    formLayout->addRow(u8"Имя юриста:", surnameEdit);
    formLayout->addRow(u8"Отчество юриста:", patronymicEdit);
    formLayout->addRow(u8"Тема:", theameEdit);
    formLayout->addRow(u8"ИНН (12 цифр):", innEdit);
    formLayout->addRow(u8"Дата (DD.MM.YYYY):", dateEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

QString AddConsultationDialog::getINN() const {
    return innEdit->text();
}

QString AddConsultationDialog::getFIO() const {
    return nameEdit->text() + " " + surnameEdit->text() + " " + patronymicEdit->text();
}

QString AddConsultationDialog::getDate() const {
    return dateEdit->text();
}

QString AddConsultationDialog::getTheame() const {
    return theameEdit->text();
}