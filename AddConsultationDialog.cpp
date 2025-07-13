#include "AddConsultationDialog.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

AddConsultationDialog::AddConsultationDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Добавить консультацию");

    fioEdit = new QLineEdit(this);
    innEdit = new QLineEdit(this);
    theameEdit = new QLineEdit(this);
    dateEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddConsultationDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddConsultationDialog::reject);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"ФИО юриста:", fioEdit);
    formLayout->addRow(u8"Тема:", theameEdit);
    formLayout->addRow(u8"ИНН клиента (12 цифр):", innEdit);
    formLayout->addRow(u8"Дата (DD.MM.YYYY):", dateEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

QString AddConsultationDialog::getINN() const {
    return innEdit->text().trimmed();
}

QString AddConsultationDialog::getFIO() const {
    return fioEdit->text().trimmed();
}

QString AddConsultationDialog::getDate() const {
    return dateEdit->text().trimmed();
}

QString AddConsultationDialog::getTheame() const {
    return theameEdit->text().trimmed();
}