#include "reportdialog.h"
#include "datamanager.h" // Уже включает в себя #define MAX_RECORDS

#include <QCheckBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDate>
#include <QFile>
#include <QTextStream>

ReportDialog::ReportDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle(u8"Формирование отчета");
    setMinimumSize(800, 600);
}

void ReportDialog::setupUi()
{
    dateEdit = new QLineEdit(this);
    dateEdit->setPlaceholderText(u8"Введите дату в формате ДД.ММ.ГГГГ");

    clientFioEdit = new QLineEdit(this);
    clientFioEdit->setPlaceholderText(u8"Полное ФИО клиента (например, Иванов Иван Иванович)");

    lawyerFioEdit = new QLineEdit(this);
    lawyerFioEdit->setPlaceholderText(u8"Полное ФИО юриста (например, Петров Петр Петрович)");

    QFormLayout* filterLayout = new QFormLayout();
    QHBoxLayout* dateLayout = new QHBoxLayout();
    dateLayout->addWidget(dateEdit);
    dateLayout->addStretch();

    filterLayout->addRow(u8"Фильтровать по дате:", dateEdit);
    filterLayout->addRow(u8"ФИО клиента:", clientFioEdit);
    filterLayout->addRow(u8"ФИО юриста:", lawyerFioEdit);

    QGroupBox* filterGroup = new QGroupBox(u8"Критерии фильтрации", this);
    filterGroup->setLayout(filterLayout);

    reportTable = new QTableWidget(this);
    reportTable->setColumnCount(6);
    reportTable->setHorizontalHeaderLabels({
        u8"Дата консультации", u8"Тема консультации", u8"ФИО юриста",
        u8"ИНН клиента", u8"ФИО клиента", u8"Телефон клиента"
        });
    reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reportTable->verticalHeader()->setVisible(false);
    reportTable->setSortingEnabled(true);

    generateButton = new QPushButton(u8"Сформировать отчет", this);
    saveButton = new QPushButton(u8"Сохранить в файл", this);
    closeButton = new QPushButton(u8"Закрыть", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(closeButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(filterGroup);
    mainLayout->addWidget(reportTable, 1);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(generateButton, &QPushButton::clicked, this, &ReportDialog::onGenerateReport);
    connect(saveButton, &QPushButton::clicked, this, &ReportDialog::onSaveReport);
    connect(closeButton, &QPushButton::clicked, this, &ReportDialog::accept);
}

void ReportDialog::onGenerateReport()
{
    FilterCriteria criteria;

    criteria.client_fio = clientFioEdit->text().trimmed();
    criteria.lawyer_fio = lawyerFioEdit->text().trimmed();
    QString dateText = dateEdit->text().trimmed();

    if (dateText.isEmpty() && criteria.client_fio.isEmpty() && criteria.lawyer_fio.isEmpty()) {
        QMessageBox::information(this, u8"Результат", u8"Не задан ни один критерий.");
        reportTable->setRowCount(0); // Очищаем таблицу, если критериев нет
        return;
    }

    if (!dateText.isEmpty()) {
        if (Validator::validateDate(dateText)) {
            criteria.date = Date(dateText);
        }
        else {
            QMessageBox::warning(this, u8"Ошибка ввода", u8"Формат даты неверен. Пожалуйста, используйте ДД.ММ.ГГГГ.");
            return;
        }
    }
    else {
        criteria.date.year = 0;
    }

    // ИЗМЕНЕНО: Используем статический массив вместо QVector
    static ReportEntry reportDataArray[MAX_RECORDS];
    // ИЗМЕНЕНО: Вызываем обновленную функцию generateReport
    int reportSize = DataManager::generateReport(criteria, reportDataArray, MAX_RECORDS);

    reportTable->setRowCount(0);

    // ИЗМЕНЕНО: Проверяем размер, а не is_empty()
    if (reportSize == 0) {
        QMessageBox::information(this, u8"Результат", u8"По заданным критериям ничего не найдено.");
        return;
    }

    reportTable->setSortingEnabled(false);

    // ИЗМЕНЕНО: Используем классический цикл for
    for (int i = 0; i < reportSize; ++i) {
        const ReportEntry& entry = reportDataArray[i]; // Для удобства
        int newRow = reportTable->rowCount();
        reportTable->insertRow(newRow);

        reportTable->setItem(newRow, 0, new QTableWidgetItem(entry.date.toString()));
        reportTable->setItem(newRow, 1, new QTableWidgetItem(entry.topic));
        reportTable->setItem(newRow, 2, new QTableWidgetItem(entry.lawyer_fio.toString()));
        reportTable->setItem(newRow, 3, new QTableWidgetItem(QString("%1").arg(entry.client_inn, 12, 10, QChar('0'))));
        reportTable->setItem(newRow, 4, new QTableWidgetItem(entry.client_fio.toString()));
        reportTable->setItem(newRow, 5, new QTableWidgetItem(QString::number(entry.phone)));
    }

    reportTable->setSortingEnabled(true);
}

void ReportDialog::onSaveReport()
{
    if (reportTable->rowCount() == 0) {
        QMessageBox::warning(this, u8"Ошибка", u8"Отчет пуст. Нечего сохранять.");
        return;
    }

    QFile file("report.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, u8"Ошибка файла", u8"Не удалось открыть файл report.txt для записи.");
        return;
    }

    QTextStream out(&file);

    for (int row = 0; row < reportTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < reportTable->columnCount(); ++col) {
            rowData << reportTable->item(row, col)->text();
        }
        out << rowData.join(";") << "\n";
    }

    file.close();
    QMessageBox::information(this, u8"Успех", u8"Отчет успешно сохранен в файл report.txt.");
}