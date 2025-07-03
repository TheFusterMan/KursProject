#pragma once

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class AddConsultationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddConsultationDialog(QWidget* parent = nullptr);

    QString getINN() const;
    QString getFIO() const;
    QString getDate() const;
    QString getTheame() const;

private:
    QLineEdit* innEdit;
    // ИЗМЕНЕНО: Заменяем три поля на одно
    QLineEdit* fioEdit;
    QLineEdit* theameEdit;
    QLineEdit* dateEdit;
    QDialogButtonBox* buttonBox;
};