#pragma once

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class AddClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddClientDialog(QWidget* parent = nullptr);

    QString getINN() const;
    QString getFIO() const;
    QString getPhone() const;

private:
    QLineEdit* innEdit;
    // ИЗМЕНЕНО: Заменяем три поля на одно
    QLineEdit* fioEdit;
    QLineEdit* phoneEdit;
    QDialogButtonBox* buttonBox;
};