#include "mainwindow.h"
#include "AddClientDialog.h"
#include "AddConsultationDialog.h"
#include "DeleteClientDialog.h"
#include "ReportDialog.h"
#include "DataManager.h"

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
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::KursProjectClass)
{
    ui->setupUi(this);

    setWindowIcon(QIcon("logo.png"));

    connect(ui->actionReport, &QAction::triggered, this, &MainWindow::onGenerateReport);
    connect(ui->actionAddClient, &QAction::triggered, this, &MainWindow::onAddClientRecord);
    connect(ui->actionDeleteClient, &QAction::triggered, this, &MainWindow::onDeleteClientRecord);
    connect(ui->actionLoadClients, &QAction::triggered, this, &MainWindow::onLoadClients);
    connect(ui->actionSaveClients, &QAction::triggered, this, &MainWindow::onSaveClients);
    connect(ui->actionFindClient, &QAction::triggered, this, &MainWindow::onFindClient);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    connect(ui->actionAddConsultation, &QAction::triggered, this, &MainWindow::onAddConsultationRecord);
    connect(ui->actionDeleteConsultation, &QAction::triggered, this, &MainWindow::onDeleteConsultationRecord);
    connect(ui->actionLoadConsultations, &QAction::triggered, this, &MainWindow::onLoadConsultations);
    connect(ui->actionSaveConsultations, &QAction::triggered, this, &MainWindow::onSaveConsultations);
    connect(ui->actionFindConsultations, &QAction::triggered, this, &MainWindow::onFindConsultations);

    connect(ui->sellersTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showClientContextMenu);
    connect(ui->salesTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showConsultationContextMenu);
    connect(ui->debugPushButton, &QPushButton::clicked, this, &MainWindow::onDebugButtonClicked);

    ui->mainTabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui; // Освобождаем память, выделенную для UI
}


void MainWindow::updateClientsTable()
{
    ui->sellersTable->setUpdatesEnabled(false);
    ui->sellersTable->setRowCount(0);

    const auto& clients = DataManager::getClients();

    for (const Client& client : clients) {
        int newRowIndex = ui->sellersTable->rowCount();
        ui->sellersTable->insertRow(newRowIndex);

        ui->sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(QString::number(client.inn)));
        ui->sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(client.fio.toString()));
        ui->sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(QString::number(client.phone)));
    }

    ui->sellersTable->setUpdatesEnabled(true);
}

void MainWindow::updateConsultationsTable()
{
    ui->salesTable->setUpdatesEnabled(false);
    ui->salesTable->setRowCount(0);

    const auto& consultations = DataManager::getConsultations();

    for (const Consultation& cons : consultations) {
        int newRow = ui->salesTable->rowCount();
        ui->salesTable->insertRow(newRow);

        ui->salesTable->setItem(newRow, 0, new QTableWidgetItem(QString::number(cons.client_inn)));
        ui->salesTable->setItem(newRow, 1, new QTableWidgetItem(cons.topic));
        ui->salesTable->setItem(newRow, 2, new QTableWidgetItem(cons.lawyer_fio.toString()));
        ui->salesTable->setItem(newRow, 3, new QTableWidgetItem(cons.date.toString()));
    }
    ui->salesTable->setUpdatesEnabled(true);
}

void MainWindow::showClientContextMenu(const QPoint& pos)
{
    QTableWidgetItem* item = ui->sellersTable->itemAt(pos);
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
            QString inn = ui->sellersTable->item(row, 0)->text();
            QString fio = ui->sellersTable->item(row, 1)->text();
            QString phone = ui->sellersTable->item(row, 2)->text();

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

    contextMenu.exec(ui->sellersTable->mapToGlobal(pos));
}

void MainWindow::showConsultationContextMenu(const QPoint& pos)
{
    QTableWidgetItem* item = ui->salesTable->itemAt(pos);
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

    contextMenu.exec(ui->salesTable->mapToGlobal(pos));
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
            int newRowIndex = ui->sellersTable->rowCount();
            ui->sellersTable->insertRow(newRowIndex);

            ui->sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(INN));
            ui->sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(FIO));
            ui->sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(Phone));

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
            int newRowIndex = ui->salesTable->rowCount();
            ui->salesTable->insertRow(newRowIndex);

            ui->salesTable->setItem(newRowIndex, 0, new QTableWidgetItem(INN));
            ui->salesTable->setItem(newRowIndex, 1, new QTableWidgetItem(Theame));
            ui->salesTable->setItem(newRowIndex, 2, new QTableWidgetItem(FIO));
            ui->salesTable->setItem(newRowIndex, 3, new QTableWidgetItem(Date));

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
    QList<QTableWidgetItem*> selectedItems = ui->salesTable->selectedItems();
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

void MainWindow::onFindClient()
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

        for (int i = 0; i < ui->sellersTable->rowCount(); ++i) {
            if (ui->sellersTable->item(i, 0)->text() == innStr) {
                ui->sellersTable->selectRow(i);
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
    //bool ok;
    //QString innStr = QInputDialog::getText(this,
    //    u8"Найти консультации клиента",
    //    u8"Введите ИНН клиента для поиска:",
    //    QLineEdit::Normal,
    //    "",
    //    &ok);

    //if (!ok || innStr.isEmpty()) {
    //    return;
    //}

    //if (!Validator::validateINN(innStr)) {
    //    QMessageBox::warning(this, u8"Ошибка ввода", u8"ИНН должен состоять ровно из 12 цифр.");
    //    return;
    //}

    //quint64 innToFind = innStr.toULongLong();

    //const CustomVector foundConsultations = DataManager::findConsultationIndicesByINN(innToFind);

    //if (!foundConsultations.isEmpty()) {
    //    QString message = QString(u8"Консультации найдены найден:\n\n"
    //        u8"ИНН: %1\n"
    //        u8"ФИО: %2\n"
    //        u8"Телефон: %3")
    //        .arg(foundClient->inn)
    //        .arg(foundClient->fio.toString())
    //        .arg(foundClient->phone);
    //    QMessageBox::information(this, u8"Результат поиска", message);

    //    for (int i = 0; i < ui->sellersTable->rowCount(); ++i) {
    //        if (ui->sellersTable->item(i, 0)->text() == innStr) {
    //            ui->sellersTable->selectRow(i);
    //            break;
    //        }
    //    }

    //}
    //else {
    //    QMessageBox::information(this, u8"Результат поиска", u8"Клиент с таким ИНН не найден.");
    //}
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

void MainWindow::onDebugButtonClicked()
{
    QString debugOutput;

    debugOutput += "========== ХЕШ-ТАБЛИЦА КЛИЕНТОВ (по ИНН) ==========\n";
    debugOutput += DataManager::getClientsTableState();
    debugOutput += "\n\n";

    debugOutput += "========== ДЕРЕВО КОНСУЛЬТАЦИЙ (по ИНН клиента) ==========\n";
    debugOutput += DataManager::getConsultationsTreeState();
    debugOutput += "\n\n";

    debugOutput += "========== ДЕРЕВО ФИЛЬТРАЦИИ (по дате) ==========\n";
    debugOutput += DataManager::getFilterTreeByDateState();

    ui->debugTextEdit->setPlainText(debugOutput);

    QMessageBox::information(this, "Отладка", "Данные структур выведены на вкладку 'Отладка'.");
}