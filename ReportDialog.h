#pragma once
#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

class QCheckBox;
class QDateEdit;
class QLineEdit;
class QPushButton;
class QTableWidget;

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

    QPushButton* generateButton;
    QPushButton* saveButton;
    QPushButton* closeButton;

    QTableWidget* reportTable;
};

#endif // REPORTDIALOG_H