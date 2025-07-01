#include "DeleteClientDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

DeleteClientDialog::DeleteClientDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Удалить клиента");

    innEdit = new QLineEdit(this);
    nameEdit = new QLineEdit(this);
    surnameEdit = new QLineEdit(this);
    patronymicEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DeleteClientDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DeleteClientDialog::reject);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"Фамилия:", nameEdit);
    formLayout->addRow(u8"Имя:", surnameEdit);
    formLayout->addRow(u8"Отчество:", patronymicEdit);
    formLayout->addRow(u8"ИНН (12 цифр):", innEdit);
    formLayout->addRow(u8"Телефон (формат 89...):", phoneEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

QString DeleteClientDialog::getINN() const {
    return innEdit->text().trimmed();
}

QString DeleteClientDialog::getFIO() const {
    return nameEdit->text().trimmed() + " " + surnameEdit->text().trimmed() + " " + patronymicEdit->text().trimmed();
}

QString DeleteClientDialog::getPhone() const {
    return phoneEdit->text().trimmed();
}