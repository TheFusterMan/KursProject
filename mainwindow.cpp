    #include "mainwindow.h"
    #include "AddClientDialog.h"
    #include "AddConsultationDialog.h"
    #include "DeleteClientDialog.h"
    #include "ReportDialog.h"
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

        sellersTable = new QTableWidget(this); // Создаем таблицу
        sellersTable->setColumnCount(3);       // Устанавливаем 3 колонки: ИНН, ФИО, Телефон

        // Устанавливаем заголовки колонок
        sellersTable->setHorizontalHeaderLabels({ u8"ИНН", u8"ФИО", u8"Телефон" });

        // Настраиваем внешний вид
        sellersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Растягиваем колонки
        sellersTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запрещаем редактирование ячеек
        sellersTable->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделение строк целиком
        sellersTable->verticalHeader()->setVisible(false); // Скрываем нумерацию строк слева

        // <<< ИЗМЕНЕНИЕ 1: ВКЛЮЧАЕМ КОНТЕКСТНОЕ МЕНЮ И ПОДКЛЮЧАЕМ СИГНАЛ >>>
        sellersTable->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(sellersTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showClientContextMenu);

        // Правая таблица (Справочник консультаций, из "ДЕРЕВО")
        salesTable = new QTableWidget(this); // Создаем таблицу
        salesTable->setColumnCount(4);       // Устанавливаем 4 колонки

        // Устанавливаем заголовки колонок
        salesTable->setHorizontalHeaderLabels({ u8"ИНН Клиента", u8"Тема консультации", u8"ФИО Юриста", u8"Дата" });

        // Настраиваем внешний вид
        salesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        salesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        salesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        salesTable->verticalHeader()->setVisible(false);

        salesTable->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(salesTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showConsultationContextMenu);

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

    // Реализации слотов
    void MainWindow::showClientContextMenu(const QPoint& pos)
    {
        // Получаем индекс элемента, на который кликнули
        QTableWidgetItem* item = sellersTable->itemAt(pos);
        if (!item) {
            return; // Клик был на пустом месте, ничего не делаем
        }
        int row = item->row();

        // Создаем меню
        QMenu contextMenu(this);
        QAction* deleteAction = contextMenu.addAction(QIcon::fromTheme("edit-delete"), u8"Удалить клиента");

        // Соединяем сигнал от действия "Удалить" с лямбда-функцией, которая выполнит удаление
        connect(deleteAction, &QAction::triggered, [this, row]() {
            // Запрашиваем подтверждение
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, u8"Подтверждение удаления",
                u8"Вы уверены, что хотите удалить этого клиента?\n"
                u8"Все связанные с ним консультации также будут удалены.",
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // Получаем данные прямо из строки таблицы для удаления
                QString inn = sellersTable->item(row, 0)->text();
                QString fio = sellersTable->item(row, 1)->text();
                QString phone = sellersTable->item(row, 2)->text();

                if (DataManager::deleteClient(inn, fio, phone)) {
                    // Если DataManager удаляет и связанные консультации,
                    // нужно обновить обе таблицы
                    updateClientsTable();
                    updateConsultationsTable();
                    QMessageBox::information(this, u8"Успех", u8"Клиент успешно удален.");
                }
                else {
                    QMessageBox::warning(this, u8"Ошибка", u8"Не удалось удалить клиента.");
                }
            }
            });

        // Показываем меню в месте, где был сделан клик
        // mapToGlobal переводит координаты из системы виджета в глобальные экранные
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
                // DataManager::deleteConsultation уже принимает индекс строки, что идеально нам подходит
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
                // --- НАЧАЛО ИЗМЕНЕНИЙ ---

                // 1. Получаем текущее количество строк. Это будет индекс для новой строки.
                int newRowIndex = salesTable->rowCount();

                // 2. Вставляем новую пустую строку в конец таблицы.
                salesTable->insertRow(newRowIndex);

                // 3. Создаем и заполняем ячейки для новой строки.
                salesTable->setItem(newRowIndex, 0, new QTableWidgetItem(INN));
                salesTable->setItem(newRowIndex, 1, new QTableWidgetItem(Theame));
                salesTable->setItem(newRowIndex, 2, new QTableWidgetItem(FIO));
                salesTable->setItem(newRowIndex, 3, new QTableWidgetItem(Date));

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

    void MainWindow::onDeleteClientRecord()
    {
        // 1. Создаем и показываем наш новый диалог
        DeleteClientDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            // 2. Если пользователь нажал "OK", получаем данные из диалога
            QString inn = dialog.getINN();
            QString fio = dialog.getFIO();
            QString phone = dialog.getPhone();

            // 3. Вызываем наш новый метод в DataManager для удаления
            if (DataManager::deleteClient(inn, fio, phone))
            {
                // 4. В случае успеха - обновляем таблицу в UI и сообщаем пользователю
                updateClientsTable(); // Эта функция перерисует таблицу с актуальными данными
                QMessageBox::information(this, u8"Успех", u8"Запись успешно удалена.");
            }
            else
            {
                // 5. В случае неудачи - сообщаем пользователю, что запись не найдена или данные не совпали
                QMessageBox::warning(this, u8"Ошибка",
                    u8"Не удалось удалить запись.\n"
                    u8"Убедитесь, что все поля (ИНН, ФИО, телефон) введены верно и полностью совпадают с существующей записью.");
            }
        }
        // Если пользователь нажал "Отмена", ничего не происходит
    }
    void MainWindow::onDeleteConsultationRecord()
    {
        // Удаляем по выделенной строке
        QList<QTableWidgetItem*> selectedItems = salesTable->selectedItems();
        if (selectedItems.isEmpty()) {
            QMessageBox::information(this, "Удаление", "Пожалуйста, выберите строку для удаления.");
            return;
        }

        int row = selectedItems.first()->row();

        // Спрашиваем подтверждение
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
            // 6. Если DataManager вернул false, значит, произошла ошибка.
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
        // 1. Запрашиваем у пользователя ИНН для поиска
        bool ok;
        QString innStr = QInputDialog::getText(this,
            u8"Найти клиента",
            u8"Введите ИНН клиента для поиска:",
            QLineEdit::Normal,
            "",
            &ok);

        // 2. Проверяем, нажал ли пользователь "OK" и ввел ли что-нибудь
        if (!ok || innStr.isEmpty()) {
            return; // Пользователь нажал "Отмена" или ничего не ввел
        }

        // 3. Валидируем введенный ИНН
        if (!Validator::validateINN(innStr)) {
            QMessageBox::warning(this, u8"Ошибка ввода", u8"ИНН должен состоять ровно из 12 цифр.");
            return;
        }

        quint64 innToFind = innStr.toULongLong();

        // 4. Используем наш новый метод в DataManager для поиска
        const Client* foundClient = DataManager::findClientByINN(innToFind);

        // 5. Отображаем результат
        if (foundClient != nullptr) {
            // Если клиент найден, выводим информацию о нем в MessageBox
            QString message = QString(u8"Клиент найден:\n\n"
                u8"ИНН: %1\n"
                u8"ФИО: %2\n"
                u8"Телефон: %3")
                .arg(foundClient->inn)
                .arg(foundClient->fio.toString())
                .arg(foundClient->phone);
            QMessageBox::information(this, u8"Результат поиска", message);

            // Дополнительно: можно выделить найденную строку в таблице
            for (int i = 0; i < sellersTable->rowCount(); ++i) {
                if (sellersTable->item(i, 0)->text() == innStr) {
                    sellersTable->selectRow(i);
                    break;
                }
            }

        }
        else {
            // Если метод вернул nullptr, значит клиент не найден
            QMessageBox::information(this, u8"Результат поиска", u8"Клиент с таким ИНН не найден.");
        }
    }
    void MainWindow::onFindConsultations()
    {
        // Этот метод может выводить все консультации для конкретного клиента в отдельном окне,
        // но для простоты мы просто отфильтруем текущую таблицу.
        // Для полноценной реализации лучше сделать отдельный диалог.
        QMessageBox::information(this, "Информация", "Эта функция пока не реализована.\nДля поиска воспользуйтесь загрузкой данных и визуальным осмотром таблицы.");
    }

    void MainWindow::onGenerateReport()
    {
        // Создаем и показываем диалог отчета.
        // Поскольку он модальный, код дальше не выполнится, пока диалог не закроется.
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