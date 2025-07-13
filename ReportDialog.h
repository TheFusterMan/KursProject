#pragma once

#include <QDialog>

// Прямые объявления для уменьшения зависимостей в заголовке
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

    // UI elements
    QLineEdit* dateEdit;
    QLineEdit* clientFioEdit;
    QLineEdit* lawyerFioEdit;
    QTableWidget* reportTable;
    QPushButton* generateButton;
    QPushButton* saveButton;
    QPushButton* closeButton;
};