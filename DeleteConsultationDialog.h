#pragma once

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

// ����� �����: ������ ��� �������� ������������ �� ������� ����������
class DeleteConsultationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteConsultationDialog(QWidget* parent = nullptr);

    QString getClientINN() const;
    QString getLawyerFIO() const;
    QString getTopic() const;
    QString getDate() const;

private:
    QLineEdit* clientInnEdit;
    QLineEdit* lawyerFioEdit;
    QLineEdit* topicEdit;
    QLineEdit* dateEdit;
    QDialogButtonBox* buttonBox;
};