#include "AddClientDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

AddClientDialog::AddClientDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Добавить клиента");

    // ИЗМЕНЕНО: Создаем одно поле для ФИО вместо трех
    fioEdit = new QLineEdit(this);
    innEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddClientDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddClientDialog::reject);

    // ИЗМЕНЕНО: Обновляем форму
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"ФИО (через пробел):", fioEdit);
    formLayout->addRow(u8"ИНН (12 цифр):", innEdit);
    formLayout->addRow(u8"Телефон (формат 89...):", phoneEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

QString AddClientDialog::getINN() const {
    return innEdit->text().trimmed();
}

QString AddClientDialog::getFIO() const {
    // ИЗМЕНЕНО: Просто возвращаем текст из одного поля
    return fioEdit->text().trimmed();
}

QString AddClientDialog::getPhone() const {
    return phoneEdit->text().trimmed();
}