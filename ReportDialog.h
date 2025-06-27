// reportdialog.h
#pragma once
#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

// Предварительные объявления для ускорения компиляции
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
    // Слот для обработки нажатия на кнопку "Сформировать отчет"
    void onGenerateReport();

private:
    void setupUi(); // Метод для создания и компоновки интерфейса

    // Виджеты для фильтров
    QCheckBox* dateFilterCheckbox;
    QDateEdit* dateEdit;
    QLineEdit* clientFioEdit;
    QLineEdit* lawyerFioEdit;

    // Кнопки
    QPushButton* generateButton;
    QPushButton* closeButton;

    // Таблица для отображения результатов
    QTableWidget* reportTable;
};

#endif // REPORTDIALOG_H