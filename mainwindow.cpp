#include "mainwindow.h"
#include "ui_KursProject.h" // Убедитесь, что имя файла верное
#include "datamanager.h"
#include "addclientdialog.h"
#include "addconsultationdialog.h"
#include "deleteclientdialog.h"
#include "deleteconsultationdialog.h"
#include "reportdialog.h"

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
#include <QDialog>
#include <QTextEdit>

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
    connect(ui->actionDeleteConsultation, &QAction::triggered, this, &MainWindow::onDeleteConsultationBySearch);
    connect(ui->actionLoadConsultations, &QAction::triggered, this, &MainWindow::onLoadConsultations);
    connect(ui->actionSaveConsultations, &QAction::triggered, this, &MainWindow::onSaveConsultations);
    connect(ui->actionFindConsultations, &QAction::triggered, this, &MainWindow::onFindConsultations);

    connect(ui->sellersTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showClientContextMenu);
    connect(ui->salesTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showConsultationContextMenu);

    connect(ui->action_2, &QAction::triggered, this, &MainWindow::onDebugActionTriggered);

    ui->mainTabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateClientsTable()
{
    ui->sellersTable->setUpdatesEnabled(false);
    ui->sellersTable->setRowCount(0);

    // ИЗМЕНЕНО: Используем прямой доступ к статическому массиву и его размеру
    for (int i = 0; i < DataManager::clients_array_size; ++i) {
        const Client& client = DataManager::clients_array[i];
        int newRowIndex = ui->sellersTable->rowCount();
        ui->sellersTable->insertRow(newRowIndex);

        ui->sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(QString("%1").arg(client.inn, 12, 10, QChar('0'))));
        ui->sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(client.fio.toString()));
        ui->sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(QString::number(client.phone)));
    }

    ui->sellersTable->setUpdatesEnabled(true);
}

void MainWindow::updateConsultationsTable()
{
    ui->salesTable->setUpdatesEnabled(false);
    ui->salesTable->setRowCount(0);

    // ИЗМЕНЕНО: Используем прямой доступ к статическому массиву и его размеру
    for (int i = 0; i < DataManager::consultations_array_size; ++i) {
        const Consultation& cons = DataManager::consultations_array[i];
        int newRow = ui->salesTable->rowCount();
        ui->salesTable->insertRow(newRow);

        ui->salesTable->setItem(newRow, 0, new QTableWidgetItem(QString("%1").arg(cons.client_inn, 12, 10, QChar('0'))));
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
    QAction* deleteAction = contextMenu.addAction(QIcon::fromTheme("edit-delete"), "Удалить клиента");

    connect(deleteAction, &QAction::triggered, [this, row]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение удаления",
            "Вы уверены, что хотите удалить этого клиента?\n"
            "Все связанные с ним консультации также будут удалены.",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString inn = ui->sellersTable->item(row, 0)->text();

            if (DataManager::deleteClientByINN(inn)) {
                updateClientsTable();
                updateConsultationsTable();
                QMessageBox::information(this, "Успех", "Клиент и его консультации успешно удалены.");
            }
            else {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить клиента. Возможно, он был изменен или удален ранее.");
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
    QAction* deleteAction = contextMenu.addAction(QIcon::fromTheme("edit-delete"), "Удалить консультацию");

    connect(deleteAction, &QAction::triggered, [this, row]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение удаления", "Вы уверены, что хотите удалить эту консультацию?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString inn = ui->salesTable->item(row, 0)->text();
            QString topic = ui->salesTable->item(row, 1)->text();
            QString lawyerFio = ui->salesTable->item(row, 2)->text();
            QString date = ui->salesTable->item(row, 3)->text();

            if (DataManager::deleteConsultationByMatch(inn, lawyerFio, topic, date)) {
                updateConsultationsTable();
                QMessageBox::information(this, "Успех", "Запись успешно удалена.");
            }
            else {
                QMessageBox::critical(this, "Ошибка", "Произошла ошибка при удалении записи. Возможно, она была изменена.");
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
            updateClientsTable();
            QMessageBox::information(this, "Успех", "Запись успешно добавлена!");
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить запись! Проверьте корректность данных.");
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
            updateConsultationsTable();
            QMessageBox::information(this, "Успех", "Запись успешно добавлена!");
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить запись! Проверьте корректность данных.");
        }
    }
}

void MainWindow::onDeleteClientRecord()
{
    DeleteClientDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString inn = dialog.getINN();

        int steps = 0;
        const Client* clientExists = DataManager::findClientByINN(inn.toULongLong(), steps);

        if (!clientExists) {
            QMessageBox::warning(this, "Ошибка", "Не удалось найти клиента с указанным ИНН.");
            return;
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение удаления",
            "Вы уверены, что хотите удалить клиента с ИНН " + inn + "?\n"
            "Все связанные с ним консультации также будут удалены!",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        if (DataManager::deleteClientByINN(inn))
        {
            updateClientsTable();
            updateConsultationsTable();
            QMessageBox::information(this, "Успех", "Клиент и все его консультации успешно удалены.");
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить клиента. Возможно, он был изменен или удален ранее.");
        }
    }
}

void MainWindow::onDeleteConsultationBySearch()
{
    DeleteConsultationDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString inn = dialog.getClientINN();
        QString fio = dialog.getLawyerFIO();
        QString topic = dialog.getTopic();
        QString date = dialog.getDate();

        if (DataManager::deleteConsultationByMatch(inn, fio, topic, date)) {
            updateConsultationsTable();
            QMessageBox::information(this, "Успех", "Консультация успешно удалена.");
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить консультацию с указанными параметрами.");
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
        QString inn = ui->salesTable->item(row, 0)->text();
        QString topic = ui->salesTable->item(row, 1)->text();
        QString lawyerFio = ui->salesTable->item(row, 2)->text();
        QString date = ui->salesTable->item(row, 3)->text();

        if (DataManager::deleteConsultationByMatch(inn, lawyerFio, topic, date)) {
            updateConsultationsTable();
            QMessageBox::information(this, "Успех", "Запись успешно удалена.");
        }
        else {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при удалении записи.");
        }
    }
}

void MainWindow::onLoadClients() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Загрузить справочник клиентов",
        "",
        "Файлы клиентов (clients_*.txt)");

    if (fileName.isEmpty()) return;

    if (DataManager::loadClientsFromFile(fileName)) {
        updateClientsTable();
        QMessageBox::information(this, "Успех", "Данные клиентов успешно загружены.");
    }
    else
    {
        QMessageBox::warning(this, "Ошибка загрузки",
            "Не удалось загрузить данные из файла.\n"
            "Пожалуйста, проверьте, что файл существует и имеет корректный формат.");
    }
}

void MainWindow::onLoadConsultations()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Загрузить справочник консультаций",
        "",
        "Файлы консультаций (consultations_*.txt)");

    if (fileName.isEmpty()) return;

    if (DataManager::loadConsultationsFromFile(fileName)) {
        updateConsultationsTable();
        QMessageBox::information(this, "Успех", "Данные консультаций успешно загружены.");
    }
    else {
        QMessageBox::warning(this, "Ошибка загрузки",
            "Не удалось загрузить данные из файла.\n"
            "Пожалуйста, проверьте, что файл существует и имеет корректный формат.");
    }
}

void MainWindow::onSaveClients()
{
    // ИЗМЕНЕНО: Проверка размера массива
    if (DataManager::clients_array_size == 0) {
        QMessageBox::information(this, "Сохранение", "Справочник клиентов пуст. Нечего сохранять.");
        return;
    }

    QString dirPath = QFileDialog::getExistingDirectory(this,
        "Сохранить справочник клиентов",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty()) return;

    if (DataManager::saveClientsToFile(dirPath + "/clients_output.txt"))
    {
        QMessageBox::information(this, "Успех", "Справочник клиентов успешно сохранен.");
    }
    else
    {
        QMessageBox::critical(this, "Ошибка сохранения",
            "Не удалось сохранить данные в файл.\n");
    }
}

void MainWindow::onSaveConsultations()
{
    // ИЗМЕНЕНО: Проверка размера массива
    if (DataManager::consultations_array_size == 0) {
        QMessageBox::information(this, "Сохранение", "Справочник консультаций пуст. Нечего сохранять.");
        return;
    }

    QString dirPath = QFileDialog::getExistingDirectory(this,
        "Сохранить справочник консультаций",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty()) return;

    if (DataManager::saveConsultationsToFile(dirPath + "/consultations_output.txt")) {
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
        "Найти клиента",
        "Введите ИНН клиента для поиска:",
        QLineEdit::Normal,
        "",
        &ok);

    if (!ok || innStr.isEmpty()) {
        return;
    }

    if (!Validator::validateINN(innStr)) {
        QMessageBox::warning(this, "Ошибка ввода", "ИНН должен состоять ровно из 12 цифр.");
        return;
    }

    int steps = 0;
    quint64 innToFind = innStr.toULongLong();

    const Client* foundClient = DataManager::findClientByINN(innToFind, steps);

    if (foundClient != nullptr) {
        QString formattedInn = QString("%1").arg(foundClient->inn, 12, 10, QChar('0'));
        QString message = QString("Клиент найден за %1 шаг(ов):\n\n"
            "ИНН: %2\n"
            "ФИО: %3\n"
            "Телефон: %4")
            .arg(steps)
            .arg(formattedInn)
            .arg(foundClient->fio.toString())
            .arg(foundClient->phone);
        QMessageBox::information(this, "Результат поиска", message);

        ui->sellersTable->clearSelection();
        for (int i = 0; i < ui->sellersTable->rowCount(); ++i) {
            if (ui->sellersTable->item(i, 0)->text() == innStr) {
                ui->sellersTable->selectRow(i);
                ui->mainTabWidget->setCurrentIndex(0);
                break;
            }
        }

    }
    else {
        QMessageBox::information(this, "Результат поиска", "Клиент с таким ИНН не найден.");
    }
}

void MainWindow::onFindConsultations()
{
    bool ok;
    QString innStr = QInputDialog::getText(this,
        "Найти консультации клиента",
        "Введите ИНН клиента для поиска его консультаций:",
        QLineEdit::Normal,
        "",
        &ok);

    if (!ok || innStr.isEmpty()) {
        return;
    }

    if (!Validator::validateINN(innStr)) {
        QMessageBox::warning(this, "Ошибка ввода", "ИНН должен состоять ровно из 12 цифр.");
        return;
    }

    int steps = 0;
    quint64 innToFind = innStr.toULongLong();

    // ИЗМЕНЕНО: Используем статический массив для получения индексов
    int indices[MAX_RECORDS];
    int numIndices = DataManager::findConsultationIndicesByINN(innToFind, indices, MAX_RECORDS, steps);


    if (numIndices > 0) {
        QString resultMessage = QString("Найдено %1 консультаций за %2 шаг(ов):\n\n")
            .arg(numIndices)
            .arg(steps);

        ui->salesTable->clearSelection();

        // ИЗМЕНЕНО: Классический цикл for вместо for-each
        for (int i = 0; i < numIndices; ++i) {
            int index = indices[i];
            if (index >= 0 && index < DataManager::consultations_array_size) {
                // ИЗМЕНЕНО: Прямой доступ к массиву консультаций
                const Consultation& cons = DataManager::consultations_array[index];
                resultMessage += QString("Тема: %1\nЮрист: %2\nДата: %3\n\n")
                    .arg(cons.topic)
                    .arg(cons.lawyer_fio.toString())
                    .arg(cons.date.toString());

                for (int j = 0; j < ui->salesTable->rowCount(); ++j) {
                    if (ui->salesTable->item(j, 0)->text() == innStr &&
                        ui->salesTable->item(j, 1)->text() == cons.topic &&
                        ui->salesTable->item(j, 3)->text() == cons.date.toString())
                    {
                        ui->salesTable->selectRow(j);
                    }
                }
            }
        }

        QMessageBox::information(this, "Результат поиска", resultMessage.trimmed());
        ui->mainTabWidget->setCurrentIndex(1);
    }
    else {
        QString message = QString("Консультации для клиента с ИНН %1 не найдены.\nПоиск выполнен за %2 шаг(ов).")
            .arg(innStr)
            .arg(steps);
        QMessageBox::information(this, "Результат поиска", message);
    }
}


void MainWindow::onGenerateReport()
{
    // Эта функция должна ТОЛЬКО создавать и показывать диалог.
    // Вся логика генерации отчета находится внутри самого ReportDialog.
    ReportDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this,
        "О программе",
        "Это приложение для курсовой работы, позволяющая работать со справочниками в юридической сфере. Сделал студент Пожидаев Д.А. Б9123-09.03.04 (3,4)");
}

void MainWindow::onDebugActionTriggered()
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

    QDialog* debugDialog = new QDialog(this);
    debugDialog->setWindowTitle("Отладка");
    debugDialog->resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(debugDialog);

    QTextEdit* textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setPlainText(debugOutput);

    layout->addWidget(textEdit);

    debugDialog->exec();

    delete debugDialog;
}