#include "mainwindow.h"
#include "AddClientDialog.h"
#include "DataManager.h"

// Включаем все необходимые заголовки
#include <QTabWidget>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout> // Для горизонтальной компоновки
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)  
{
    setWindowIcon(QIcon("logo.png"));

    setupUi();
}

MainWindow::~MainWindow()
{
    // В C++, дочерние QObject'ы автоматически удаляются при уничтожении их родителя.
    // Ручное удаление здесь не требуется.
}

void MainWindow::setupUi()
{
    // 1. Настройка главного окна
    this->setWindowTitle(u8"Курсовая работа");
    this->resize(800, 600);

    // 2. Создание действий и меню
    createActions();
    createMenus();

    // 3. Создание центрального виджета и основного QTabWidget
    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainTabWidget = new QTabWidget();
    mainLayout->addWidget(mainTabWidget);

    // --- Создание вкладок ---

    // Создаем одну вкладку с двумя таблицами, как на скриншоте
    QWidget* directoriesTab = new QWidget();
    QHBoxLayout* directoriesLayout = new QHBoxLayout(directoriesTab);

    // Левая таблица (Справочник продавцов, из "ХТ")
    // Примечание: на скриншоте "ХТ" и "ДЕРЕВО" находятся внутри таблиц, а не в заголовках.
    // Пока что мы используем их как текст-заполнитель.
    sellersTable = new QTableWidget(this); // Создаем таблицу
    sellersTable->setColumnCount(3);       // Устанавливаем 3 колонки: ИНН, ФИО, Телефон

    // Устанавливаем заголовки колонок
    sellersTable->setHorizontalHeaderLabels({ u8"ИНН", u8"ФИО", u8"Телефон" });

    // Настраиваем внешний вид
    sellersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Растягиваем колонки
    sellersTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запрещаем редактирование ячеек
    sellersTable->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделение строк целиком
    sellersTable->verticalHeader()->setVisible(false); // Скрываем нумерацию строк слева

    // Правая таблица (Справочник продаж, из "ДЕРЕВО")
    salesTable = new QTableWidget(1, 1); // 1 строка, 1 колонка
    salesTable->setItem(0, 0, new QTableWidgetItem(u8"ДЕРЕВО"));
    salesTable->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    salesTable->horizontalHeader()->setVisible(false);
    salesTable->verticalHeader()->setVisible(false);
    salesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Добавляем обе таблицы в горизонтальный слой
    directoriesLayout->addWidget(sellersTable);
    directoriesLayout->addWidget(salesTable);

    // Добавляем подготовленную вкладку в QTabWidget
    mainTabWidget->addTab(directoriesTab, u8"Справочники");

    // При необходимости можно добавить другие вкладки
    QWidget* reportTab = new QWidget();
    mainTabWidget->addTab(reportTab, u8"Отчет");

    // Устанавливаем вкладку "Справочники" как текущую
    mainTabWidget->setCurrentIndex(0);
}

void MainWindow::createActions()
{
    displayAction = new QAction(u8"Показать справочник", this);
    addAction = new QAction(u8"Добавить клиента", this);
    deleteAction = new QAction(u8"Удалить запись", this);
    loadAction = new QAction(u8"Загрузить", this);
    saveAction = new QAction(u8"Сохранить", this);
    debugAction = new QAction(u8"Отладка", this);
    findAction = new QAction(u8"Найти", this);
    exitAction = new QAction(u8"Выход", this);
    aboutAction = new QAction(u8"О программе", this);

    connect(addAction, &QAction::triggered, this, &MainWindow::onAddRecord);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteRecord);
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoad);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);
    connect(debugAction, &QAction::triggered, this, &MainWindow::onDebug);
    connect(findAction, &QAction::triggered, this, &MainWindow::onFind);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(u8"&Файл");
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(u8"&Правка");
    editMenu->addAction(findAction);
    editMenu->addAction(displayAction);
    editMenu->addSeparator();
    editMenu->addAction(addAction);
    editMenu->addAction(deleteAction);

    helpMenu = menuBar()->addMenu(u8"&Справка");
    helpMenu->addAction(aboutAction);
}

void MainWindow::updateClientsTable()
{
    // Отключаем обновления для производительности
    sellersTable->setUpdatesEnabled(false);

    // Полностью очищаем таблицу перед заполнением
    sellersTable->setRowCount(0);

    // Получаем данные из нашего менеджера
    const auto& clients = DataManager::getClients();

    // Заполняем таблицу данными
    for (const Client& client : clients) {
        int newRowIndex = sellersTable->rowCount();
        sellersTable->insertRow(newRowIndex);

        sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(QString::number(client.inn)));
        sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(client.fio.toString()));
        sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(QString::number(client.phone)));
    }

    // Включаем обновления обратно, таблица перерисуется один раз
    sellersTable->setUpdatesEnabled(true);
}


// Реализации слотов

void MainWindow::onAddRecord()
{
    AddClientDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString INN = dialog.getINN();
        QString FIO = dialog.getFIO();
        QString Phone = dialog.getPhone();

        if (DataManager::addClient(INN, FIO, Phone))
        {
            // --- НАЧАЛО ИЗМЕНЕНИЙ ---

            // 1. Получаем текущее количество строк. Это будет индекс для новой строки.
            int newRowIndex = sellersTable->rowCount();

            // 2. Вставляем новую пустую строку в конец таблицы.
            sellersTable->insertRow(newRowIndex);

            // 3. Создаем и заполняем ячейки для новой строки.
            sellersTable->setItem(newRowIndex, 0, new QTableWidgetItem(INN));
            sellersTable->setItem(newRowIndex, 1, new QTableWidgetItem(FIO));
            sellersTable->setItem(newRowIndex, 2, new QTableWidgetItem(Phone));

            // Сообщение пользователю оставляем
            QMessageBox::information(this, u8"Успех", u8"Запись успешно добавлена!");

            // --- КОНЕЦ ИЗМЕНЕНИЙ ---
        }
        else
        {
            QMessageBox::warning(this, u8"Ошибка", u8"Не удалось добавить запись! Проверьте корректность данных.");
        }
    }
}


void MainWindow::onDeleteRecord() { qDebug() << u8"Нажато 'Удалить запись'!"; }
void MainWindow::onLoad() {
    // 1. Открываем диалог для выбора файла.
    // Мы просим пользователя выбрать текстовый файл для загрузки.
    QString fileName = QFileDialog::getOpenFileName(this,
        u8"Загрузить справочник клиентов", // Заголовок окна
        "",                                // Стартовая директория (текущая)
        u8"Текстовые файлы (*.txt);;Все файлы (*.*)"); // Фильтр файлов

    // 2. Проверяем, выбрал ли пользователь файл.
    // Если он нажал "Отмена", fileName будет пустым.
    if (fileName.isEmpty()) {
        return; // Ничего не делаем, если пользователь отменил выбор
    }

    // 3. Пытаемся загрузить данные из выбранного файла через DataManager.
    if (DataManager::loadClientsFromFile(fileName))
    {
        // 4. Если загрузка прошла успешно, обновляем нашу таблицу в интерфейсе.
        updateClientsTable();

        // 5. Сообщаем пользователю об успехе.
        QMessageBox::information(this, u8"Успех", u8"Данные клиентов успешно загружены.");
    }
    else
    {
        // 6. Если DataManager вернул false, значит, произошла ошибка.
        QMessageBox::warning(this, u8"Ошибка загрузки",
            u8"Не удалось загрузить данные из файла.\n"
            u8"Пожалуйста, проверьте, что файл существует и имеет корректный формат.");
    }
}
void MainWindow::onSave()
{
    // 1. Проверяем, есть ли вообще что сохранять.
    if (DataManager::getClients().isEmpty()) {
        QMessageBox::information(this, u8"Сохранение", u8"Справочник клиентов пуст. Нечего сохранять.");
        return;
    }

    // 2. Открываем диалог "Сохранить как...".
    // Он отличается от диалога открытия, но используется так же.
    QString fileName = QFileDialog::getSaveFileName(this,
        u8"Сохранить справочник клиентов", // Заголовок окна
        "clients.txt",                     // Имя файла по умолчанию
        u8"Текстовые файлы (*.txt);;Все файлы (*.*)"); // Фильтр файлов

    // 3. Проверяем, выбрал ли пользователь имя файла.
    if (fileName.isEmpty()) {
        return; // Пользователь нажал "Отмена"
    }

    // 4. Вызываем наш новый метод в DataManager для сохранения данных.
    if (DataManager::saveClientsToFile(fileName))
    {
        // 5. Сообщаем пользователю об успехе.
        QMessageBox::information(this, u8"Успех", u8"Справочник клиентов успешно сохранен.");
    }
    else
    {
        // 6. Сообщаем об ошибке, если DataManager вернул false.
        QMessageBox::critical(this, u8"Ошибка сохранения",
            u8"Не удалось сохранить данные в файл.\n"
            u8"Возможно, у вас нет прав на запись в эту директорию.");
    }
}
void MainWindow::onDebug() { qDebug() << u8"Нажато 'Отладка'!"; }
void MainWindow::onFind() //НАДО СВОЮ ФОРМУ ПРОСТО РЕАЛИЗОВАТЬ
{
    //// 1. Запрашиваем у пользователя ИНН для поиска
    //bool ok;
    //QString innStr = QInputDialog::getText(this, u8"Найти клиента", u8"Введите ИНН клиента:", QLineEdit::Normal, "", &ok);

    //// 2. Проверяем ввод
    //if (!ok || innStr.isEmpty()) {
    //    return; // Пользователь нажал "Отмена" или ничего не ввел
    //}

    //// 3. Валидируем ИНН
    //if (!Validator::validateINN(innStr)) {
    //    QMessageBox::warning(this, u8"Ошибка ввода", u8"ИНН должен состоять ровно из 12 цифр.");
    //    return;
    //}

    //quint64 innToFind = innStr.toULongLong();

    //// 4. Используем наш новый метод в DataManager для поиска
    //const Client* foundClient = DataManager::findClientByINN(innToFind);

    //// 5. Отображаем результат
    //if (foundClient != nullptr) {
    //    // Если клиент найден, выводим информацию о нем
    //    QString message = QString(u8"Клиент найден:\n\nИНН: %1\nФИО: %2\nТелефон: %3")
    //        .arg(foundClient->inn)
    //        .arg(foundClient->fio.toString())
    //        .arg(foundClient->phone);
    //    QMessageBox::information(this, u8"Результат поиска", message);
    //}
    //else {
    //    // Если метод вернул nullptr, значит клиент не найден
    //    QMessageBox::information(this, u8"Результат поиска", u8"Клиент с таким ИНН не найден.");
    //}
}

void MainWindow::onAbout()
{
    QMessageBox::about(this,
        u8"О программе",
        u8"Это приложение для курсовой работы, демонстрирующее\n"
        u8"возможности компоновки виджетов в Qt.");
}