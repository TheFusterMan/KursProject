#include "AddConsultationDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout> // Идеально подходит для форм "метка: поле"
#include <QVBoxLayout>
#include <QDialogButtonBox>

AddConsultationDialog::AddConsultationDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Добавить консультацию");

    // Создаем поля для ввода

    innEdit = new QLineEdit(this);
    nameEdit = new QLineEdit(this);
    surnameEdit = new QLineEdit(this);
    patronymicEdit = new QLineEdit(this);
    theameEdit = new QLineEdit(this);
    dateEdit = new QLineEdit(this);

    // Создаем стандартные кнопки OK и Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // Соединяем сигналы кнопок со стандартными слотами QDialog
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddConsultationDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddConsultationDialog::reject);

    // Используем QFormLayout для удобного расположения полей и меток
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"Фамилия юриста:", nameEdit);
    formLayout->addRow(u8"Имя юриста:", surnameEdit);
    formLayout->addRow(u8"Отчество юриста:", patronymicEdit);
    formLayout->addRow(u8"Тема:", theameEdit);
    formLayout->addRow(u8"ИНН (12 цифр):", innEdit);
    formLayout->addRow(u8"Дата (DD.MM.YYYY):", dateEdit);

    // Основной вертикальный компоновщик
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    // Устанавливаем главный компоновщик для диалога
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