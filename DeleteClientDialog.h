#pragma once

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class DeleteClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteClientDialog(QWidget* parent = nullptr);

    QString getINN() const;

private:
    // ИЗМЕНЕНО: Оставляем только поле для ИНН
    QLineEdit* innEdit;
    QDialogButtonBox* buttonBox;
};