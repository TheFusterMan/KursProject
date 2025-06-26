#include "DeleteClientDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

DeleteClientDialog::DeleteClientDialog(QWidget* parent) : QDialog(parent)
{
    // Устанавливаем заголовок, соответствующий действию
    setWindowTitle(u8"Удалить клиента");

    // Создаем поля для ввода
    innEdit = new QLineEdit(this);
    nameEdit = new QLineEdit(this);
    surnameEdit = new QLineEdit(this);
    patronymicEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);

    // Создаем стандартные кнопки OK и Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // Соединяем сигналы кнопок со стандартными слотами QDialog
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DeleteClientDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DeleteClientDialog::reject);

    // Используем QFormLayout для удобного расположения полей и меток
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"Фамилия:", nameEdit);
    formLayout->addRow(u8"Имя:", surnameEdit);
    formLayout->addRow(u8"Отчество:", patronymicEdit);
    formLayout->addRow(u8"ИНН (12 цифр):", innEdit);
    formLayout->addRow(u8"Телефон (формат 89...):", phoneEdit);

    // Основной вертикальный компоновщик
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    // Устанавливаем главный компоновщик для диалога
    setLayout(mainLayout);
}

QString DeleteClientDialog::getINN() const {
    return innEdit->text().trimmed();
}

QString DeleteClientDialog::getFIO() const {
    // Собираем ФИО из трех полей
    return nameEdit->text().trimmed() + " " + surnameEdit->text().trimmed() + " " + patronymicEdit->text().trimmed();
}

QString DeleteClientDialog::getPhone() const {
    return phoneEdit->text().trimmed();
}