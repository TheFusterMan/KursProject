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
// ДОБАВЛЕНО: Эти заголовки нужны для создания диалога программно
#include <QDialog>
#include <QTextEdit>
#include <QFontDatabase>


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

    // ЭТА СТРОКА БЫЛА ИСТОЧНИКОМ ОШИБКИ, ТАК КАК ui->debugPushButton БОЛЬШЕ НЕ СУЩЕСТВУЕТ
    // connect(ui->debugPushButton, &QPushButton::clicked, this, &MainWindow::onDebugActionTriggered); // <--- УДАЛИТЕ ИЛИ ЗАКОММЕНТИРУЙТЕ ЭТУ СТРОКУ

    // Этот connect остается, так как action_2 все еще существует в UI файле (в меню "Правка")
    connect(ui->action_2, &QAction::triggered, this, &MainWindow::onDebugActionTriggered);

    ui->mainTabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui; // Освобождаем память, выделенную для UI
}

// ... (весь ваш код до onAbout без изменений) ...

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
            // ИЗМЕНЕНО: Получаем только ИНН, так как для удаления больше ничего не нужно.
            QString inn = ui->sellersTable->item(row, 0)->text();

            // ИЗМЕНЕНО: Вызываем новую функцию deleteClientByINN, которая принимает только ИНН.
            if (DataManager::deleteClientByINN(inn)) {
                updateClientsTable();
                updateConsultationsTable(); // Консультации тоже обновляем, т.к. они удаляются вместе с клиентом
                QMessageBox::information(this, u8"Успех", u8"Клиент и его консультации успешно удалены.");
            }
            else {
                // Сообщение об ошибке тоже можно сделать более точным
                QMessageBox::warning(this, u8"Ошибка", u8"Не удалось удалить клиента. Возможно, он был изменен или удален ранее.");
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

        // Добавим подтверждение, т.к. операция стала более разрушительной
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, u8"Подтверждение удаления",
            u8"Вы уверены, что хотите удалить клиента с ИНН " + inn + "?\n"
            u8"Все связанные с ним консультации также будут удалены!",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        if (DataManager::deleteClientByINN(inn))
        {
            updateClientsTable();
            updateConsultationsTable(); // Нужно обновить обе таблицы
            QMessageBox::information(this, u8"Успех", u8"Клиент и все его консультации успешно удалены.");
        }
        else
        {
            QMessageBox::warning(this, u8"Ошибка", u8"Не удалось найти клиента с указанным ИНН.");
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
            QMessageBox::information(this, u8"Успех", u8"Консультация успешно удалена.");
        }
        else {
            QMessageBox::warning(this, u8"Ошибка", u8"Не удалось найти консультацию с указанными параметрами.");
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

    int steps = 0;
    quint64 innToFind = innStr.toULongLong();

    const Client* foundClient = DataManager::findClientByINN(innToFind, steps);

    if (foundClient != nullptr) {
        QString message = QString(u8"Клиент найден за %1 шаг(ов):\n\n"
            u8"ИНН: %2\n"
            u8"ФИО: %3\n"
            u8"Телефон: %4")
            .arg(steps)
            .arg(foundClient->inn)
            .arg(foundClient->fio.toString())
            .arg(foundClient->phone);
        QMessageBox::information(this, u8"Результат поиска", message);

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
        QMessageBox::information(this, u8"Результат поиска", u8"Клиент с таким ИНН не найден.");
    }
}

void MainWindow::onFindConsultations()
{
    bool ok;
    QString innStr = QInputDialog::getText(this,
        u8"Найти консультации клиента",
        u8"Введите ИНН клиента для поиска его консультаций:",
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

    int steps = 0; // Переменная для подсчета шагов
    quint64 innToFind = innStr.toULongLong();

    // ИЗМЕНЕНО: Вызываем функцию с передачей счетчика шагов
    CustomVector<int> indices = DataManager::findConsultationIndicesByINN(innToFind, steps);
    const auto& allConsultations = DataManager::getConsultations();

    if (!indices.isEmpty()) {
        QString resultMessage = QString(u8"Найдено %1 консультаций за %2 шаг(ов):\n\n")
            .arg(indices.size())
            .arg(steps);

        ui->salesTable->clearSelection(); // Очищаем предыдущее выделение

        for (int index : indices) {
            if (index >= 0 && index < allConsultations.size()) {
                const Consultation& cons = allConsultations.at(index);
                resultMessage += QString(u8"Тема: %1\nЮрист: %2\nДата: %3\n\n")
                    .arg(cons.topic)
                    .arg(cons.lawyer_fio.toString())
                    .arg(cons.date.toString());

                // Подсветка найденных строк в таблице
                // Этот цикл предполагает, что строки в таблице соответствуют индексам в массиве
                for (int i = 0; i < ui->salesTable->rowCount(); ++i) {
                    if (ui->salesTable->item(i, 0)->text() == innStr &&
                        ui->salesTable->item(i, 1)->text() == cons.topic &&
                        ui->salesTable->item(i, 3)->text() == cons.date.toString())
                    {
                        ui->salesTable->selectRow(i);
                    }
                }
            }
        }

        QMessageBox::information(this, u8"Результат поиска", resultMessage.trimmed());
        ui->mainTabWidget->setCurrentIndex(1); // Переключаемся на вкладку с консультациями
    }
    else {
        // Сообщение, если консультации не найдены (но поиск по дереву все равно был)
        QString message = QString(u8"Консультации для клиента с ИНН %1 не найдены.\nПоиск выполнен за %2 шаг(ов).")
            .arg(innStr)
            .arg(steps);
        QMessageBox::information(this, u8"Результат поиска", message);
    }
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

// ИЗМЕНЕНО: Старая функция onDebugButtonClicked переименована и полностью переписана
void MainWindow::onDebugActionTriggered()
{
    // 1. Собираем отладочную информацию, как и раньше
    QString debugOutput;
    debugOutput += "========== ХЕШ-ТАБЛИЦА КЛИЕНТОВ (по ИНН) ==========\n";
    debugOutput += DataManager::getClientsTableState();
    debugOutput += "\n\n";
    debugOutput += "========== ДЕРЕВО КОНСУЛЬТАЦИЙ (по ИНН клиента) ==========\n";
    debugOutput += DataManager::getConsultationsTreeState();
    debugOutput += "\n\n";
    debugOutput += "========== ДЕРЕВО ФИЛЬТРАЦИИ (по дате) ==========\n";
    debugOutput += DataManager::getFilterTreeByDateState();

    // 2. Создаем новое диалоговое окно программно
    QDialog* debugDialog = new QDialog(this); // Указываем родителя для корректного управления памятью
    debugDialog->setWindowTitle(u8"Окно отладки");
    debugDialog->resize(800, 600); // Задаем удобный размер

    // 3. Создаем компоновщик для окна
    QVBoxLayout* layout = new QVBoxLayout(debugDialog);

    // 4. Создаем текстовое поле для вывода информации
    QTextEdit* textEdit = new QTextEdit();
    textEdit->setReadOnly(true); // Только для чтения
    textEdit->setPlainText(debugOutput); // Помещаем в него собранный текст

    // Для лучшей читаемости используем моноширинный шрифт
    textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    // 5. Добавляем текстовое поле в компоновщик
    layout->addWidget(textEdit);

    // 6. Показываем окно в модальном режиме
    debugDialog->exec();

    // 7. После закрытия окна, удаляем его, чтобы избежать утечек памяти
    delete debugDialog;
}