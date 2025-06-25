#include "AddClientDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout> // Идеально подходит для форм "метка: поле"
#include <QVBoxLayout>
#include <QDialogButtonBox>

AddClientDialog::AddClientDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Добавить клиента");

    // Создаем поля для ввода
    innEdit = new QLineEdit(this);
    nameEdit = new QLineEdit(this);
    surnameEdit = new QLineEdit(this);
    patronymicEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);

    // Создаем стандартные кнопки OK и Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // Соединяем сигналы кнопок со стандартными слотами QDialog
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddClientDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddClientDialog::reject);

    // Используем QFormLayout для удобного расположения полей и меток
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"Фамилия:", nameEdit);
    formLayout->addRow(u8"Имя:", surnameEdit);
    formLayout->addRow(u8"Отчество:", patronymicEdit);
    formLayout->addRow(u8"ИНН:", innEdit);
    formLayout->addRow(u8"Телефон:", phoneEdit);

    // Основной вертикальный компоновщик
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    // Устанавливаем главный компоновщик для диалога
    setLayout(mainLayout);
}

QString AddClientDialog::getINN() const {
    return innEdit->text();
}

QString AddClientDialog::getFIO() const {
    return nameEdit->text() + " " + surnameEdit->text() + " " + patronymicEdit->text();
}

QString AddClientDialog::getPhone() const {
    return phoneEdit->text();
}