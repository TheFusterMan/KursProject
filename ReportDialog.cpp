#include "reportdialog.h" // Предполагается, что reportdialog.h существует
#include "datamanager.h" // Используем DataManager
#include "CustomVector.h"

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

ReportDialog::ReportDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle(u8"Формирование отчета");
    setMinimumSize(800, 600); // Немного увеличим размер для удобства
}

void ReportDialog::setupUi()
{
    dateFilterCheckbox = new QCheckBox(u8"Фильтровать по дате:", this);
    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");
    dateEdit->setEnabled(false);
    dateFilterCheckbox->setChecked(false);

    clientFioEdit = new QLineEdit(this);
    clientFioEdit->setPlaceholderText(u8"Часть ФИО клиента (регистр не важен)");
    lawyerFioEdit = new QLineEdit(this);
    lawyerFioEdit->setPlaceholderText(u8"Часть ФИО юриста (регистр не важен)");

    QFormLayout* filterLayout = new QFormLayout();
    QHBoxLayout* dateLayout = new QHBoxLayout();
    dateLayout->addWidget(dateFilterCheckbox);
    dateLayout->addWidget(dateEdit);
    dateLayout->addStretch();

    filterLayout->addRow(dateLayout);
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
    reportTable->setSortingEnabled(true); // Включим сортировку по колонкам

    generateButton = new QPushButton(u8"Сформировать отчет", this);
    closeButton = new QPushButton(u8"Закрыть", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(closeButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(filterGroup);
    mainLayout->addWidget(reportTable, 1); // Даем таблице возможность растягиваться
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(dateFilterCheckbox, &QCheckBox::toggled, dateEdit, &QDateEdit::setEnabled);
    connect(generateButton, &QPushButton::clicked, this, &ReportDialog::onGenerateReport);
    connect(closeButton, &QPushButton::clicked, this, &ReportDialog::accept);
}

// --- НАЧАЛО ИЗМЕНЕНИЙ ---
void ReportDialog::onGenerateReport()
{
    FilterCriteria criteria;

    if (dateFilterCheckbox->isChecked()) {
        QDate qd = dateEdit->date();
        criteria.date.day = qd.day();
        criteria.date.month = qd.month();
        criteria.date.year = qd.year();
    }
    else {
        // Устанавливаем год в 0, чтобы показать, что фильтр по дате неактивен
        criteria.date.year = 0;
    }

    criteria.client_fio = clientFioEdit->text().trimmed();
    criteria.lawyer_fio = lawyerFioEdit->text().trimmed();

    CustomVector<ReportEntry> reportData = DataManager::generateReport(criteria);

    reportTable->setRowCount(0); // Очищаем таблицу перед заполнением

    if (reportData.isEmpty()) {
        QMessageBox::information(this, u8"Результат", u8"По заданным критериям ничего не найдено.");
        return;
    }

    reportTable->setSortingEnabled(false); // Отключаем сортировку на время заполнения для производительности

    for (const auto& entry : reportData) {
        int newRow = reportTable->rowCount();
        reportTable->insertRow(newRow);

        // Поля извлекаются напрямую из "плоской" структуры ReportEntry
        reportTable->setItem(newRow, 0, new QTableWidgetItem(entry.date.toString()));
        reportTable->setItem(newRow, 1, new QTableWidgetItem(entry.topic));
        reportTable->setItem(newRow, 2, new QTableWidgetItem(entry.lawyer_fio.toString()));
        reportTable->setItem(newRow, 3, new QTableWidgetItem(QString::number(entry.client_inn)));
        reportTable->setItem(newRow, 4, new QTableWidgetItem(entry.client_fio.toString()));
        reportTable->setItem(newRow, 5, new QTableWidgetItem(QString::number(entry.phone)));
    }

    reportTable->setSortingEnabled(true); // Включаем сортировку обратно
}
// --- КОНЕЦ ИЗМЕНЕНИЙ ---