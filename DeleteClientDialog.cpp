#include "DeleteClientDialog.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

DeleteClientDialog::DeleteClientDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Удалить клиента по ИНН");

    innEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

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