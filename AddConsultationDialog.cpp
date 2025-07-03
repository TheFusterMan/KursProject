#include "AddConsultationDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

AddConsultationDialog::AddConsultationDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Добавить консультацию");

    // ИЗМЕНЕНО: Создаем одно поле для ФИО юриста
    fioEdit = new QLineEdit(this);
    innEdit = new QLineEdit(this);
    theameEdit = new QLineEdit(this);
    dateEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddConsultationDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddConsultationDialog::reject);

    // ИЗМЕНЕНО: Обновляем форму
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
    // ИЗМЕНЕНО: Просто возвращаем текст из одного поля
    return fioEdit->text().trimmed();
}

QString AddConsultationDialog::getDate() const {
    return dateEdit->text().trimmed();
}

QString AddConsultationDialog::getTheame() const {
    return theameEdit->text().trimmed();
}