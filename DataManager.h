#pragma once
#include <HashTable.h>
#include <AVLTree.h>

#include <QString>
#include <stdexcept>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream> // Added for QTextStream
#include <QVector>

struct Date {
    int day;
    int month;
    int year;

    QString toString() const {
        return QString("%1.%2.%3")
            .arg(day, 2, 10, QChar('0'))
            .arg(month, 2, 10, QChar('0'))
            .arg(year, 4, 10, QChar('0'));
    }
};

struct FIO {
    QString f;
    QString i;
    QString o;

    FIO() = default;

    // Конструктор из строки fio
    // Ключевое слово 'explicit' предотвращает неявные преобразования типов
    explicit FIO(const QString& fullName) {
        // 1. Разделяем строку по пробелам.
        // Qt::SkipEmptyParts удаляет пустые строки, которые могут появиться,
        // если между словами несколько пробелов или в начале/конце строки.
        const QStringList parts = fullName.split(' ', Qt::SkipEmptyParts);

        // 2. Присваиваем части структуры, проверяя их наличие.
        // .at() безопаснее, чем [], так как не вызовет падения при выходе за границы.
        if (parts.size() > 0) {
            f = parts.at(0);
        }
        if (parts.size() > 1) {
            i = parts.at(1);
        }
        if (parts.size() > 2) {
            o = parts.at(2);
        }
        // Если в строке больше 3 слов, остальные просто игнорируются.
    }

    QString toString() const {
        return QString(f + " " + i + " " + o);
    }
};

struct Client {
    quint64 inn;
    FIO fio;
    quint64 phone;
};

struct Consultation {
    quint64 client_inn;
    QString topic;
    FIO lawyer_fio;
    Date date;

    QString toFileLine() const {
        return QString("%1;%2;%3;%4")
            .arg(client_inn)
            .arg(topic)
            .arg(lawyer_fio.toString())
            .arg(date.toString());
    }
};

struct FilterCriteria
{
    Date date;
    QString client_fio;
    QString lawyer_fio;
};

struct ReportEntry
{
    Date date;
    FIO lawyer_fio;
    FIO client_fio;
};

static class Validator {
public:
    static bool validateINN(const QString& inn) {
        static const QRegularExpression innRegex(QStringLiteral("^\\d{12}$"));
        if (!innRegex.match(inn).hasMatch()) {
            return false;
            //throw std::invalid_argument("INN must consist of exactly 12 digits."); // Original: "ИНН должен состоять ровно из 12 цифр."
        }
        return true;
    }

    static bool validateFIO(const QString& fio) {
        // Original: "ФИО должно состоять из трех слов на русском языке, с заглавной буквы каждое, разделенных пробелом."
        static const QRegularExpression fioRegex(QStringLiteral("^[А-ЯЁ][а-яё]+ [А-ЯЁ][а-яё]+ [А-ЯЁ][а-яё]+$"));
        if (!fioRegex.match(fio).hasMatch()) {
            return false;
        }
        return true;
    }

    static bool validatePhone(const QString& phone) {
        // Телефон должен состоять из 11 цифр и начинаться с "89".
        // u8 здесь не обязателен, т.к. нет не-ASCII символов, но для единообразия полезен.
        static const QRegularExpression phoneRegex(u8"^89\\d{9}$");
        if (!phoneRegex.match(phone).hasMatch()) {
            return false;
        }
        return true;
    }

    static bool validateDate(int day, int month, int year) {
        if (year < 1970 || year > 2038) {
            return false;
        }
        if (month < 1 || month > 12) {
            return false;
        }

        int max_days = 31;
        if (month == 4 || month == 6 || month == 9 || month == 11) {
            max_days = 30;
        }
        else if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            max_days = isLeap ? 29 : 28;
        }

        if (day < 1 || day > max_days) {
            return false;
        }
        return true;
    }
};

static class DataManager {
private:
    static Validator validator;

    inline static HashTable clients_table {16};
    inline static QVector<Client> clients_array;
    
    inline static AVLTree consultations_tree;
    inline static QVector<Consultation> consultations_array;

    public:
    static bool loadClientsFromFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл с клиентами:" << filename;
            return false;
        }

        clients_array.clear();
        //clients_table.clear();

        QTextStream in(&file);
        int index = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(';');

            if (parts.size() == 3 && validator.validateINN(parts[0].trimmed()) && validator.validateFIO(parts[1].trimmed()) && validator.validatePhone(parts[2].trimmed())) {
                Client client;
                client.inn = parts[0].trimmed().toULongLong();
                client.phone = parts[2].trimmed().toULongLong();
                QStringList fioParts = parts[1].trimmed().split(' ');
                client.fio.f = fioParts[0];
                client.fio.i = fioParts[1];
                client.fio.o = fioParts[2];
                clients_array.append(client);
                clients_table.add(client.inn, index); // <-- РАСКОММЕНТИРОВАНО и исправлено
                index++;
            }
            else {
                qWarning() << "Incorrect line format in clients file:" << line; // Original: "Некорректный формат строки в файле клиентов:"
            }
        }

        file.close();
        qInfo() << "Загружено" << clients_array.size() << "клиентов.";
        return true;
    }

    static bool loadConsultationsFromFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл с консультациями:" << filename;
            return false;
        }

        // Полная очистка перед загрузкой
        consultations_array.clear();
        consultations_tree.freeTree(consultations_tree.root);

        QTextStream in(&file);
        int index = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(';');

            if (parts.size() == 4) {
                bool inn_ok, lawyer_fio_ok, date_ok;

                // 1. Валидация ИНН
                quint64 client_inn = parts[0].toULongLong(&inn_ok);
                inn_ok = inn_ok && validator.validateINN(parts[0]);

                // 2. Валидация ФИО юриста
                QString lawyer_fio_str = parts[2].trimmed();
                lawyer_fio_ok = validator.validateFIO(lawyer_fio_str);

                // 3. Валидация даты
                QStringList dateParts = parts[3].trimmed().split('.');
                Date date;
                if (dateParts.size() == 3) {
                    date.day = dateParts[0].toInt();
                    date.month = dateParts[1].toInt();
                    date.year = dateParts[2].toInt();
                    date_ok = validator.validateDate(date.day, date.month, date.year);
                }
                else {
                    date_ok = false;
                }

                if (inn_ok && lawyer_fio_ok && date_ok) {
                    Consultation consultation;
                    consultation.client_inn = client_inn;
                    consultation.topic = parts[1].trimmed();
                    consultation.lawyer_fio = FIO(lawyer_fio_str);
                    consultation.date = date;

                    // Добавляем в массив и в дерево
                    consultations_array.append(consultation);
                    bool h = false;
                    consultations_tree.insert(consultations_tree.root, consultation.client_inn, index, h);
                    index++;
                }
                else {
                    qWarning() << "Некорректный формат строки в файле консультаций:" << line;
                }
            }
            else {
                qWarning() << "Некорректное количество полей в строке файла консультаций:" << line;
            }
        }
        file.close();
        qInfo() << "Загружено" << consultations_array.size() << "консультаций.";
        return true;
    }

    static bool saveClientsToFile(const QString& filename)
    {
        QFile file(filename);
        // Открываем файл для записи. QIODevice::WriteOnly перезапишет файл, если он существует.
        // QIODevice::Text обеспечивает правильную обработку символов новой строки.
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл для записи:" << filename;
            return false;
        }

        // Создаем текстовый поток для удобной записи в файл
        QTextStream out(&file);
        //out.setCodec("UTF-8"); // Устанавливаем кодировку UTF-8 для корректной записи кириллицы

        // Проходим по всему массиву клиентов
        for (const Client& client : clients_array) {
            // Формируем строку в формате "ИНН;ФИО;Телефон"
            QString line = QString("%1;%2;%3")
                .arg(client.inn)
                .arg(client.fio.toString())
                .arg(QString::number(client.phone)); // Телефон тоже конвертируем в строку

            // Записываем строку в файл и добавляем символ новой строки
            out << line << "\n";
        }

        file.close(); // Закрываем файл после записи
        qInfo() << "Сохранено" << clients_array.size() << "клиентов в файл" << filename;
        return true;
    }

    static bool saveConsultationsToFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл для записи консультаций:" << filename;
            return false;
        }

        QTextStream out(&file);
        for (const Consultation& consultation : consultations_array) {
            out << consultation.toFileLine() << "\n";
        }

        file.close();
        qInfo() << "Сохранено" << consultations_array.size() << "консультаций в файл" << filename;
        return true;
    }

    static bool addClient(const QString& inn, const QString& fio, const QString& phone) {
        if (validator.validateINN(inn) && validator.validateFIO(fio) && validator.validatePhone(phone)) {
            quint64 validatedINN = inn.toULongLong();
            FIO validatedFIO (fio);
            quint64 validatedPhone = phone.toULongLong();

            if (clients_table.add(validatedINN, clients_array.length() - 1)) {
                clients_array.append({
                    validatedINN,
                    validatedFIO,
                    validatedPhone
                    });

                return true;
            }
        }
        return false;
    }

    static bool addConsultation(const Consultation& consultation)
    {
        // Проверяем, существует ли клиент с таким ИНН
        if (findClientByINN(consultation.client_inn) == nullptr) {
            qWarning() << "Попытка добавить консультацию для несуществующего клиента с ИНН:" << consultation.client_inn;
            return false;
        }

        int newIndex = consultations_array.size();
        consultations_array.append(consultation);
        consultations_tree.add(consultation.client_inn, newIndex);
        return true;
    }

    static bool deleteClient(const QString& inn, const QString& fio, const QString& phone)
    {
        // 1. Валидация входных данных
        if (!validator.validateINN(inn) || !validator.validateFIO(fio) || !validator.validatePhone(phone)) {
            qWarning() << "Invalid data format for deletion.";
            return false;
        }

        bool ok;
        quint64 innToDelete_q64 = inn.toULongLong(&ok);
        if (!ok) {
            qWarning() << "INN conversion to number failed:" << inn;
            return false;
        }
        // Приводим ключ к типу int, как того требует ваша хеш-таблица
        int innToDelete_int = static_cast<int>(innToDelete_q64);


        // 2. Ищем клиента по ИНН в хеш-таблице
        // Ваш метод search возвращает const Item*, что отлично подходит для поиска.
        const Item* itemToDelete = clients_table.search(innToDelete_int);

        // Если search вернул nullptr, значит ключ не найден
        if (itemToDelete == nullptr) {
            qInfo() << "Client with INN" << inn << "not found. Deletion failed.";
            return false;
        }

        int indexToDelete = itemToDelete->index;

        // Проверка на валидность индекса из хеш-таблицы
        if (indexToDelete < 0 || indexToDelete >= clients_array.size()) {
            qWarning() << "Hash table contains an invalid index" << indexToDelete << "for INN" << inn;
            return false;
        }

        // 3. Сверяем остальные данные (ФИО, телефон) для полного совпадения
        const Client& clientInArray = clients_array.at(indexToDelete);
        FIO fioFromInput(fio);
        quint64 phoneFromInput = phone.toULongLong();

        if (clientInArray.fio.toString() != fioFromInput.toString() || clientInArray.phone != phoneFromInput) {
            qInfo() << "Client with INN" << inn << "found, but FIO/phone do not match. Deletion aborted.";
            return false;
        }

        // --- ПОЛНОЕ СОВПАДЕНИЕ НАЙДЕНО. НАЧИНАЕМ ЭФФЕКТИВНОЕ УДАЛЕНИЕ ---

        int lastIndex = clients_array.size() - 1;

        // 4. Случай, когда удаляемый элемент - НЕ последний в массиве
        if (indexToDelete < lastIndex) {
            // Берем последний элемент из массива
            const Client& lastClient = clients_array.last();
            int lastClientInn_int = static_cast<int>(lastClient.inn);

            // Перемещаем его на место удаляемого элемента
            clients_array[indexToDelete] = lastClient;

            // Обновляем индекс перемещенного элемента в хеш-таблице
            if (!clients_table.updateIndex(lastClientInn_int, indexToDelete)) {
                // Эта ошибка критична - она говорит о рассинхронизации данных
                qCritical() << "CRITICAL: Inconsistency! Could not update index for client with INN"
                    << lastClient.inn << " in the hash table.";
                // В этом случае лучше не продолжать, чтобы не повредить данные еще больше
                return false;
            }
        }
        // Если удаляемый элемент и так последний, этот блок пропускается, что корректно.

        // 5. Удаляем (помечаем) запись из хеш-таблицы
        clients_table.remove(innToDelete_int);

        // 6. Удаляем последний элемент из массива (быстрая операция O(1))
        clients_array.removeLast();

        qInfo() << "Client with INN" << inn << "successfully deleted.";
        return true;
    }

    static bool deleteConsultation(int indexInArray)
    {
        if (indexInArray < 0 || indexInArray >= consultations_array.size()) {
            qWarning() << "Попытка удаления консультации по неверному индексу:" << indexInArray;
            return false;
        }

        const Consultation& toDelete = consultations_array.at(indexInArray);
        quint64 keyToDelete = toDelete.client_inn;

        int lastIndex = consultations_array.size() - 1;

        // Используем эффективный метод удаления из вектора (swap-and-pop)
        if (indexInArray < lastIndex) {
            const Consultation& lastElement = consultations_array.last();
            // Перемещаем последний элемент на место удаляемого
            consultations_array[indexInArray] = lastElement;

            // Обновляем дерево для ПЕРЕМЕЩЕННОГО элемента:
            // 1. Удаляем его старый индекс
            consultations_tree.remove(lastElement.client_inn, lastIndex);
            // 2. Добавляем его новый индекс
            consultations_tree.add(lastElement.client_inn, indexInArray);
        }

        // Удаляем последний элемент из массива
        consultations_array.removeLast();

        // Теперь удаляем из дерева ИНДЕКС исходного удаляемого элемента
        consultations_tree.remove(keyToDelete, indexInArray);

        return true;
    }


    static const QVector<Client>& getClients() 
    {
        return clients_array;
    }

    static const QVector<Consultation>& getConsultations() 
    {
        return consultations_array;
    }

    static const Client* findClientByINN(quint64 inn)
    {
        // 1. Приводим ключ к типу int, который использует ваша хеш-таблица
        int inn_int = static_cast<int>(inn);

        // 2. Ищем в хеш-таблице. Метод search вернет указатель на Item или nullptr.
        const Item* foundItem = clients_table.search(inn_int);

        // 3. Проверяем результат поиска
        if (foundItem != nullptr) { // Убеждаемся, что указатель не нулевой (т.е. клиент найден)
            int index = foundItem->index;

            // 4. Проверяем валидность индекса, полученного из хеш-таблицы
            if (index >= 0 && index < clients_array.size()) {
                // Индекс валиден, возвращаем указатель на клиента в массиве
                return &clients_array[index];
            }
            else {
                // Ошибка: в хеш-таблице хранится "мусорный" индекс.
                // Этого не должно происходить в исправной системе.
                qWarning() << "Inconsistency found: HashTable returned invalid index" << index << "for INN" << inn;
                return nullptr;
            }
        }

        // 5. Если search вернул nullptr, значит клиент не найден.
        return nullptr;
    }

    static QVector<int> findConsultationIndicesByINN(quint64 inn)
    {
        QVector<int> indices;
        TreeNode* node = consultations_tree.find(inn);
        if (node) {
            ListNode* current = node->head;
            while (current) {
                indices.append(current->data);
                current = current->next;
            }
        }
        return indices;
    }
};