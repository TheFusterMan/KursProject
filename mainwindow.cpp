#include "mainwindow.h"
#include "AddClientDialog.h"
#include "AddConsultationDialog.h"
#include "DeleteClientDialog.h"
#include "ReportDialog.h"
#include "DataManager.h"

#include <QTabWidget>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QIcon>
#include <QPoint>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon("logo.png"));
    setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    this->setWindowTitle(u8"Курсовая работа");
    this->resize(800, 600);

    createActions();
    createMenus();

    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainTabWidget = new QTabWidget();
    mainLayout->addWidget(mainTabWidget);

    QWidget* directoriesTab = new QWidget();
    QHBoxLayout* directoriesLayout = new QHBoxLayout(directoriesTab);

    sellersTable = new QTableWidget(this);
    sellersTable->setColumnCount(3);

    sellersTable->setHorizontalHeaderLabels({ u8"ИНН", u8"ФИО", u8"Телефон" });

    sellersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    sellersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sellersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    sellersTable->verticalHeader()->setVisible(false);

    sellersTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(sellersTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showClientContextMenu);

    salesTable = new QTableWidget(this);
    salesTable->setColumnCount(4);

    salesTable->setHorizontalHeaderLabels({ u8"ИНН Клиента", u8"Тема консультации", u8"ФИО Юриста", u8"Дата" });

    salesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    salesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    salesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    salesTable->verticalHeader()->setVisible(false);

    salesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(salesTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showConsultationContextMenu);

    directoriesLayout->addWidget(sellersTable);
    directoriesLayout->addWidget(salesTable);

    mainTabWidget->addTab(directoriesTab, u8"Справочники");

    QWidget* reportTab = new QWidget();
    mainTabWidget->addTab(reportTab, u8"Отчет");

    mainTabWidget->setCurrentIndex(0);
}

void MainWindow::createActions()
{
    addAction = new QAction(u8"Добавить клиента", this);
    deleteAction = new QAction(u8"Удалить клиента", this);
    loadClientsAction = new QAction(u8"Загрузить справочник Клиенты", this);
    saveClientsAction = new QAction(u8"Сохранить справочник Клиенты", this);
    debugAction = new QAction(u8"Отладка", this);
    findAction = new QAction(u8"Найти", this);
    exitAction = new QAction(u8"Выход", this);
    aboutAction = new QAction(u8"О программе", this);
    reportAction = new QAction(u8"Сформировать отчет", this);

    connect(reportAction, &QAction::triggered, this, &MainWindow::onGenerateReport);
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddClientRecord);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteClientRecord);
    connect(loadClientsAction, &QAction::triggered, this, &MainWindow::onLoadClients);
    connect(saveClientsAction, &QAction::triggered, this, &MainWindow::onSaveClients);
    connect(debugAction, &QAction::triggered, this, &MainWindow::onDebug);
    connect(findAction, &QAction::triggered, this, &MainWindow::onFind);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    addConsultationAction = new QAction(u8"Добавить консультацию", this);
    deleteConsultationAction = new QAction(u8"Удалить консультацию", this);
    loadConsultationsAction = new QAction(u8"Загрузить справочник Консультации", this);
    saveConsultationsAction = new QAction(u8"Сохранить справочник Консультации", this);
    findConsultationsAction = new QAction(u8"Найти консультации клиента", this);

    connect(addConsultationAction, &QAction::triggered, this, &MainWindow::onAddConsultationRecord);
    connect(deleteConsultationAction, &QAction::triggered, this, &MainWindow::onDeleteConsultationRecord);
    connect(loadConsultationsAction, &QAction::triggered, this, &MainWindow::onLoadConsultations);
    connect(saveConsultationsAction, &QAction::triggered, this, &MainWindow::onSaveConsultations);
    connect(findConsultationsAction, &QAction::triggered, this, &MainWindow::onFindConsultations);
}
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(u8"&Файл");
    fileMenu->addAction(loadClientsAction);
    fileMenu->addAction(loadConsultationsAction);
    fileMenu->addAction(saveClientsAction);
    fileMenu->addAction(saveConsultationsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(reportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(u8"&Правка");
    editMenu->addAction(findAction);
    editMenu->addAction(findConsultationsAction);
    editMenu->addSeparator();
    editMenu->addAction(addAction);
    editMenu->addAction(addConsultationAction);
    editMenu->addAction(deleteAction);
    editMenu->addAction(deleteConsultationAction);

    helpMenu = menuBar()->addMenu(u8"&Справка");
    helpMenu->addAction(aboutAction);
}

void MainWindow::updateClientsTable()
{
    sellersTable->setUpdatesEnabled(false);
    sellersTable->setRowCount(0);

    const auto& clients = DataManager::getClients();

    for (const Client& client : clients) {
        int newRowIndex = sellersTable->rowCount();
        sellersTable->insertRow(newRowIndex);

        sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(QString::number(client.inn)));
        sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(client.fio.toString()));
        sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(QString::number(client.phone)));
    }

    sellersTable->setUpdatesEnabled(true);
}
void MainWindow::updateConsultationsTable()
{
    salesTable->setUpdatesEnabled(false);
    salesTable->setRowCount(0);

    const auto& consultations = DataManager::getConsultations();

    for (const Consultation& cons : consultations) {
        int newRow = salesTable->rowCount();
        salesTable->insertRow(newRow);

        salesTable->setItem(newRow, 0, new QTableWidgetItem(QString::number(cons.client_inn)));
        salesTable->setItem(newRow, 1, new QTableWidgetItem(cons.topic));
        salesTable->setItem(newRow, 2, new QTableWidgetItem(cons.lawyer_fio.toString()));
        salesTable->setItem(newRow, 3, new QTableWidgetItem(cons.date.toString()));
    }
    salesTable->setUpdatesEnabled(true);
}

void MainWindow::showClientContextMenu(const QPoint& pos)
{
    QTableWidgetItem* item = sellersTable->itemAt(pos);
    if (!item) {
        return;
    }
    int row = item->row();

    QMenu contextMenu(this);
    QAction* deleteAction = contextMenu.addAction(QIcon::fromTheme("edit-delete"), u8"Удалить клиента");

    connect(deleteAction, &QAction::triggered, [this, row]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, u8"Подтверждение удаления",
            u8"Вы уверены, что хотите удалить этого клиента?\n"
            u8"Все связанные с ним консультации также будут удалены.",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString inn = sellersTable->item(row, 0)->text();
            QString fio = sellersTable->item(row, 1)->text();
            QString phone = sellersTable->item(row, 2)->text();

            if (DataManager::deleteClient(inn, fio, phone)) {
                updateClientsTable();
                updateConsultationsTable();
                QMessageBox::information(this, u8"Успех", u8"Клиент успешно удален.");
            }
            else {
                QMessageBox::warning(this, u8"Ошибка", u8"Не удалось удалить клиента.");
            }
        }
        });

    contextMenu.exec(sellersTable->mapToGlobal(pos));
}
void MainWindow::showConsultationContextMenu(const QPoint& pos)
{
    QTableWidgetItem* item = salesTable->itemAt(pos);
    if (!item) {
        return;
    }
    int row = item->row();

    QMenu contextMenu(this);
    QAction* deleteAction = contextMenu.addAction(QIcon::fromTheme("edit-delete"), u8"Удалить консультацию");

    connect(deleteAction, &QAction::triggered, [this, row]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, u8"Подтверждение удаления", u8"Вы уверены, что хотите удалить эту консультацию?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (DataManager::deleteConsultation(row)) {
                updateConsultationsTable();
                QMessageBox::information(this, u8"Успех", u8"Запись успешно удалена.");
            }
            else {
                QMessageBox::critical(this, u8"Ошибка", u8"Произошла ошибка при удалении записи.");
            }
        }
        });

    contextMenu.exec(salesTable->mapToGlobal(pos));
}

void MainWindow::onAddClientRecord()
{
    AddClientDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString INN = dialog.getINN();
        QString FIO = dialog.getFIO();
        QString Phone = dialog.getPhone();

        if (DataManager::addClient(INN, FIO, Phone))
        {
            int newRowIndex = sellersTable->rowCount();
            sellersTable->insertRow(newRowIndex);

            sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(INN));
            sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(FIO));
            sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(Phone));

            QMessageBox::information(this, u8"Успех", u8"Запись успешно добавлена!");
        }
        else
        {
            QMessageBox::warning(this, u8"Ошибка", u8"Не удалось добавить запись! Проверьте корректность данных.");
        }
    }
}
void MainWindow::onAddConsultationRecord()
{
    AddConsultationDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString INN = dialog.getINN();
        QString Date = dialog.getDate();
        QString FIO = dialog.getFIO();
        QString Theame = dialog.getTheame();

        if (DataManager::addConsultation(INN, FIO, Theame, Date))
        {
            int newRowIndex = salesTable->rowCount();
            salesTable->insertRow(newRowIndex);

            salesTable->setItem(newRowIndex, 0, new QTableWidgetItem(INN));
            salesTable->setItem(newRowIndex, 1, new QTableWidgetItem(Theame));
            salesTable->setItem(newRowIndex, 2, new QTableWidgetItem(FIO));
            salesTable->setItem(newRowIndex, 3, new QTableWidgetItem(Date));

            QMessageBox::information(this, u8"Успех", u8"Запись успешно добавлена!");
        }
        else
        {
            QMessageBox::warning(this, u8"Ошибка", u8"Не удалось добавить запись! Проверьте корректность данных.");
        }
    }
}

void MainWindow::onDeleteClientRecord()
{
    DeleteClientDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString inn = dialog.getINN();
        QString fio = dialog.getFIO();
        QString phone = dialog.getPhone();

        if (DataManager::deleteClient(inn, fio, phone))
        {
            updateClientsTable();
            QMessageBox::information(this, u8"Успех", u8"Запись успешно удалена.");
        }
        else
        {
            QMessageBox::warning(this, u8"Ошибка",
                u8"Не удалось удалить запись.\n"
                u8"Убедитесь, что все поля (ИНН, ФИО, телефон) введены верно и полностью совпадают с существующей записью.");
        }
    }
}
void MainWindow::onDeleteConsultationRecord()
{
    QList<QTableWidgetItem*> selectedItems = salesTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "Удаление", "Пожалуйста, выберите строку для удаления.");
        return;
    }

    int row = selectedItems.first()->row();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите удалить эту консультацию?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (DataManager::deleteConsultation(row)) {
            updateConsultationsTable();
            QMessageBox::information(this, "Успех", "Запись успешно удалена.");
        }
        else {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при удалении записи.");
        }
    }
}

void MainWindow::onLoadClients() {
    QString fileName = QFileDialog::getOpenFileName(this, u8"Загрузить справочник клиентов", "", u8"Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (fileName.isEmpty()) return;

    if (DataManager::loadClientsFromFile(fileName)) {
        updateClientsTable();
        QMessageBox::information(this, u8"Успех", u8"Данные клиентов успешно загружены.");
    }
    else
    {
        QMessageBox::warning(this, u8"Ошибка загрузки",
            u8"Не удалось загрузить данные из файла.\n"
            u8"Пожалуйста, проверьте, что файл существует и имеет корректный формат.");
    }
}
void MainWindow::onLoadConsultations()
{
    QString fileName = QFileDialog::getOpenFileName(this, u8"Загрузить справочник консультаций", "", u8"Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (fileName.isEmpty()) return;

    if (DataManager::loadConsultationsFromFile(fileName)) {
        updateConsultationsTable();
        QMessageBox::information(this, u8"Успех", u8"Данные консультаций успешно загружены.");
    }
    else {
        QMessageBox::warning(this, u8"Ошибка загрузки",
            u8"Не удалось загрузить данные из файла.\n"
            u8"Пожалуйста, проверьте, что файл существует и имеет корректный формат.");
    }
}

void MainWindow::onSaveClients()
{
    if (DataManager::getClients().isEmpty()) {
        QMessageBox::information(this, u8"Сохранение", u8"Справочник клиентов пуст. Нечего сохранять.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        u8"Сохранить справочник клиентов",
        "clients.txt",
        u8"Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (DataManager::saveClientsToFile(fileName))
    {
        QMessageBox::information(this, u8"Успех", u8"Справочник клиентов успешно сохранен.");
    }
    else
    {
        QMessageBox::critical(this, u8"Ошибка сохранения",
            u8"Не удалось сохранить данные в файл.\n"
            u8"Возможно, у вас нет прав на запись в эту директорию.");
    }
}
void MainWindow::onSaveConsultations()
{
    if (DataManager::getConsultations().isEmpty()) {
        QMessageBox::information(this, "Сохранение", "Справочник консультаций пуст. Нечего сохранять.");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить справочник консультаций", "consultations.txt", "Текстовые файлы (*.txt);;Все файлы (*.*)");
    if (fileName.isEmpty()) return;

    if (DataManager::saveConsultationsToFile(fileName)) {
        QMessageBox::information(this, "Успех", "Справочник консультаций успешно сохранен.");
    }
    else {
        QMessageBox::critical(this, "Ошибка сохранения", "Не удалось сохранить данные в файл.");
    }
}

void MainWindow::onDebug() { qDebug() << u8"Нажато 'Отладка'!"; }
void MainWindow::onFind()
{
    bool ok;
    QString innStr = QInputDialog::getText(this,
        u8"Найти клиента",
        u8"Введите ИНН клиента для поиска:",
        QLineEdit::Normal,
        "",
        &ok);

    if (!ok || innStr.isEmpty()) {
        return;
    }

    if (!Validator::validateINN(innStr)) {
        QMessageBox::warning(this, u8"Ошибка ввода", u8"ИНН должен состоять ровно из 12 цифр.");
        return;
    }

    quint64 innToFind = innStr.toULongLong();

    const Client* foundClient = DataManager::findClientByINN(innToFind);

    if (foundClient != nullptr) {
        QString message = QString(u8"Клиент найден:\n\n"
            u8"ИНН: %1\n"
            u8"ФИО: %2\n"
            u8"Телефон: %3")
            .arg(foundClient->inn)
            .arg(foundClient->fio.toString())
            .arg(foundClient->phone);
        QMessageBox::information(this, u8"Результат поиска", message);

        for (int i = 0; i < sellersTable->rowCount(); ++i) {
            if (sellersTable->item(i, 0)->text() == innStr) {
                sellersTable->selectRow(i);
                break;
            }
        }

    }
    else {
        QMessageBox::information(this, u8"Результат поиска", u8"Клиент с таким ИНН не найден.");
    }
}
void MainWindow::onFindConsultations()
{
    QMessageBox::information(this, "Информация", "Эта функция пока не реализована.\nДля поиска воспользуйтесь загрузкой данных и визуальным осмотром таблицы.");
}

void MainWindow::onGenerateReport()
{
    ReportDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this,
        u8"О программе",
        u8"Это приложение для курсовой работы, демонстрирующее\n"
        u8"возможности компоновки виджетов в Qt.");
}