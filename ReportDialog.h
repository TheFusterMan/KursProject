#pragma once

#include <QDialog>

class QLineEdit;
class QTableWidget;
class QPushButton;

class ReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDialog(QWidget* parent = nullptr);

private slots:
    void onGenerateReport();
    void onSaveReport();

private:
    void setupUi();

    QLineEdit* dateEdit;
    QLineEdit* clientFioEdit;
    QLineEdit* lawyerFioEdit;
    QTableWidget* reportTable;
    QPushButton* generateButton;
    QPushButton* saveButton;
    QPushButton* closeButton;
};