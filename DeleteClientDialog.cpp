#include "DeleteClientDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

DeleteClientDialog::DeleteClientDialog(QWidget* parent) : QDialog(parent)
{
    // ИЗМЕНЕНО: Обновляем заголовок
    setWindowTitle(u8"Удалить клиента по ИНН");

    innEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // ИЗМЕНЕНО: Упрощаем форму до одного поля
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"ИНН клиента (12 цифр):", innEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

QString DeleteClientDialog::getINN() const {
    return innEdit->text().trimmed();
}