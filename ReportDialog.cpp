// reportdialog.cpp
#include "reportdialog.h"
#include "datamanager.h" // Включаем наш фасад для доступа к данным

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
    setMinimumSize(700, 500);
}

void ReportDialog::setupUi()
{
    // 1. Создание виджетов для фильтров
    dateFilterCheckbox = new QCheckBox(u8"Фильтровать по дате:", this);
    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");
    dateEdit->setEnabled(false); // Изначально поле даты неактивно
    dateFilterCheckbox->setChecked(false);

    clientFioEdit = new QLineEdit(this);
    clientFioEdit->setPlaceholderText(u8"Часть ФИО клиента (регистр не важен)");
    lawyerFioEdit = new QLineEdit(this);
    lawyerFioEdit->setPlaceholderText(u8"Часть ФИО юриста (регистр не важен)");

    // 2. Создание компоновки для фильтров
    QFormLayout* filterLayout = new QFormLayout();
    filterLayout->addRow(dateFilterCheckbox, dateEdit);
    filterLayout->addRow(u8"ФИО клиента:", clientFioEdit);
    filterLayout->addRow(u8"ФИО юриста:", lawyerFioEdit);

    QGroupBox* filterGroup = new QGroupBox(u8"Критерии фильтрации", this);
    filterGroup->setLayout(filterLayout);

    // 3. Создание таблицы для результатов
    reportTable = new QTableWidget(this);
    reportTable->setColumnCount(3);
    reportTable->setHorizontalHeaderLabels({ u8"Дата", u8"ФИО Юриста", u8"ФИО Клиента" });
    reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reportTable->verticalHeader()->setVisible(false);

    // 4. Создание кнопок
    generateButton = new QPushButton(u8"Сформировать отчет", this);
    closeButton = new QPushButton(u8"Закрыть", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // Добавляем растягивающееся пространство
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(closeButton);

    // 5. Главная компоновка диалога
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(filterGroup);
    mainLayout->addWidget(reportTable);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // 6. Соединение сигналов и слотов
    connect(dateFilterCheckbox, &QCheckBox::toggled, dateEdit, &QDateEdit::setEnabled);
    connect(generateButton, &QPushButton::clicked, this, &ReportDialog::onGenerateReport);
    connect(closeButton, &QPushButton::clicked, this, &ReportDialog::accept); // accept() закрывает диалог
}

void ReportDialog::onGenerateReport()
{
    // 1. Собираем критерии из полей ввода
    FilterCriteria criteria;

    if (dateFilterCheckbox->isChecked()) {
        QDate qd = dateEdit->date();
        // Преобразуем QDate в нашу структуру Date
        criteria.date.day = qd.day();
        criteria.date.month = qd.month();
        criteria.date.year = qd.year();
    }
    else {
        // Если флажок не установлен, год=0 будет сигналом не использовать фильтр по дате
        criteria.date.year = 0;
    }

    criteria.client_fio = clientFioEdit->text().trimmed();
    criteria.lawyer_fio = lawyerFioEdit->text().trimmed();

    // 2. Вызываем метод из DataManager для получения данных
    QVector<ReportEntry> reportData = DataManager::generateReport(criteria);

    // 3. Заполняем таблицу результатами
    reportTable->setRowCount(0); // Очищаем таблицу перед заполнением

    if (reportData.isEmpty()) {
        QMessageBox::information(this, u8"Результат", u8"По заданным критериям ничего не найдено.");
        return;
    }

    for (const auto& entry : reportData) {
        int newRow = reportTable->rowCount();
        reportTable->insertRow(newRow);

        reportTable->setItem(newRow, 0, new QTableWidgetItem(entry.date.toString()));
        reportTable->setItem(newRow, 1, new QTableWidgetItem(entry.lawyer_fio.toString()));
        reportTable->setItem(newRow, 2, new QTableWidgetItem(entry.client_fio.toString()));
    }
}