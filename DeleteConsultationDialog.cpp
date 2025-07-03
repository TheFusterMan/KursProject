#include "DeleteConsultationDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

DeleteConsultationDialog::DeleteConsultationDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(u8"Удалить консультацию");

    clientInnEdit = new QLineEdit(this);
    lawyerFioEdit = new QLineEdit(this);
    topicEdit = new QLineEdit(this);
    dateEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DeleteConsultationDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DeleteConsultationDialog::reject);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(u8"ИНН клиента:", clientInnEdit);
    formLayout->addRow(u8"ФИО юриста:", lawyerFioEdit);
    formLayout->addRow(u8"Тема:", topicEdit);
    formLayout->addRow(u8"Дата (DD.MM.YYYY):", dateEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

QString DeleteConsultationDialog::getClientINN() const {
    return clientInnEdit->text().trimmed();
}

QString DeleteConsultationDialog::getLawyerFIO() const {
    return lawyerFioEdit->text().trimmed();
}

QString DeleteConsultationDialog::getTopic() const {
    return topicEdit->text().trimmed();
}

QString DeleteConsultationDialog::getDate() const {
    return dateEdit->text().trimmed();
}