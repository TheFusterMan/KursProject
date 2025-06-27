#pragma once
#include <HashTable.h>
#include <AVLTree.h>
#include "DebugLogger.h"

#include <QString>
#include <stdexcept>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream> // Added for QTextStream
#include <QVector>
#include <QDebug> // Для вывода отладочной информации

// Структуры данных (без изменений)
struct Date {
    Date() : day(0), month(0), year(0) {} // Инициализация по умолчанию

    Date(QString textParts) {
        QStringList dateParts = textParts.split('.');
        if (dateParts.size() == 3) {
            day = dateParts[0].toInt();
            month = dateParts[1].toInt();
            year = dateParts[2].toInt();
        }
        else {
            day = 0; month = 0; year = 0;
        }
    }

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

    explicit FIO(const QString& fullName) {
        const QStringList parts = fullName.split(' ', Qt::SkipEmptyParts);
        if (parts.size() > 0) f = parts.at(0);
        if (parts.size() > 1) i = parts.at(1);
        if (parts.size() > 2) o = parts.at(2);
    }

    QString toString() const {
        return QString(f + " " + i + " " + o).trimmed();
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

// Критерии для фильтрации отчета
struct FilterCriteria
{
    Date date; // Если year == 0, фильтр по дате не применяется
    QString client_fio; // Если пустая строка, фильтр не применяется
    QString lawyer_fio; // Если пустая строка, фильтр не применяется
};

// Одна запись в итоговом отчете
struct ReportEntry
{
    Date date;
    FIO lawyer_fio;
    FIO client_fio;
};

// Класс Validator (без изменений)
static class Validator {
public:
    static bool validateINN(const QString& inn) {
        static const QRegularExpression innRegex(QStringLiteral("^\\d{12}$"));
        return innRegex.match(inn).hasMatch();
    }

    static bool validateFIO(const QString& fio) {
        static const QRegularExpression fioRegex(QStringLiteral("^[А-ЯЁ][а-яё]+ [А-ЯЁ][а-яё]+ [А-ЯЁ][а-яё]+$"));
        return fioRegex.match(fio).hasMatch();
    }

    static bool validatePhone(const QString& phone) {
        static const QRegularExpression phoneRegex(u8"^89\\d{9}$");
        return phoneRegex.match(phone).hasMatch();
    }

    static bool validateDate(const QString& date) {
        QStringList dateParts = date.trimmed().split('.');
        if (dateParts.size() != 3) return false;

        bool isDayOk, isMonthOk, isYearOk;
        int day = dateParts[0].toInt(&isDayOk);
        int month = dateParts[1].toInt(&isMonthOk);
        int year = dateParts[2].toInt(&isYearOk);

        if (!isDayOk || !isMonthOk || !isYearOk) return false;
        if (year < 1970 || year > 2038) return false;
        if (month < 1 || month > 12) return false;

        int max_days = 31;
        if (month == 4 || month == 6 || month == 9 || month == 11) max_days = 30;
        else if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            max_days = isLeap ? 29 : 28;
        }

        return (day >= 1 && day <= max_days);
    }
};

static class DataManager {
private:
    static Validator validator;

    // Основные хранилища данных
    inline static HashTable clients_table{ 10 };
    inline static QVector<Client> clients_array;

    inline static AVLTree consultations_tree; // Ключ: ИНН клиента
    inline static QVector<Consultation> consultations_array;

    // --- НОВОЕ: ДЕРЕВО ФИЛЬТРАЦИИ ДЛЯ ОТЧЕТОВ ---
    inline static AVLTree filter_tree_by_date; // Ключ: Дата

    // Преобразует дату в числовой ключ для АВЛ-дерева (ГГГГММДД)
    static quint64 dateToKey(const Date& date) {
        return static_cast<quint64>(date.year) * 10000 +
            static_cast<quint64>(date.month) * 100 +
            static_cast<quint64>(date.day);
    }

    // Полностью перестраивает дерево фильтрации на основе текущего массива консультаций
    static void rebuildFilterTree() {
        filter_tree_by_date.freeTree(filter_tree_by_date.root);
        filter_tree_by_date.root = nullptr;

        for (int i = 0; i < consultations_array.size(); ++i) {
            const auto& consultation = consultations_array.at(i);
            quint64 key = dateToKey(consultation.date);
            filter_tree_by_date.add(key, i);
        }
        qInfo() << "Дерево фильтрации по дате успешно перестроено.";
    }

    // Рекурсивный обход дерева для построения отчета
    static void traverseForReport(TreeNode* node, const FilterCriteria& criteria, QVector<ReportEntry>& reportData) {
        if (!node) {
            return;
        }

        // In-order traversal (сначала левое поддерево) для сохранения сортировки по дате
        traverseForReport(node->left, criteria, reportData);

        // --- Обработка текущего узла ---
        bool dateFilterActive = criteria.date.year > 0;
        quint64 criteriaDateKey = dateFilterActive ? dateToKey(criteria.date) : 0;

        // 1. Проверяем фильтр по дате
        if (!dateFilterActive || node->key == criteriaDateKey) {
            // Дата подходит, теперь проверяем каждую консультацию в эту дату
            ListNode* currentIndexNode = node->head;
            while (currentIndexNode) {
                int consultationIndex = currentIndexNode->data;
                const Consultation& consultation = consultations_array.at(consultationIndex);
                const Client* client = findClientByINN(consultation.client_inn);

                if (client) {
                    // 2. Проверяем фильтр по ФИО клиента
                    bool clientFioMatch = criteria.client_fio.isEmpty() ||
                        client->fio.toString().contains(criteria.client_fio, Qt::CaseInsensitive);

                    // 3. Проверяем фильтр по ФИО юриста
                    bool lawyerFioMatch = criteria.lawyer_fio.isEmpty() ||
                        consultation.lawyer_fio.toString().contains(criteria.lawyer_fio, Qt::CaseInsensitive);

                    // Если все фильтры пройдены, добавляем запись в отчет
                    if (clientFioMatch && lawyerFioMatch) {
                        reportData.append({ consultation.date, consultation.lawyer_fio, client->fio });
                    }
                }
                currentIndexNode = currentIndexNode->next;
            }
        }
        // --- Конец обработки ---

        // Обходим правое поддерево
        traverseForReport(node->right, criteria, reportData);
    }

public:
    // --- НОВЫЙ ПУБЛИЧНЫЙ МЕТОД ДЛЯ ГЕНЕРАЦИИ ОТЧЕТА ---
    static QVector<ReportEntry> generateReport(const FilterCriteria& criteria) {
        QVector<ReportEntry> reportData;
        traverseForReport(filter_tree_by_date.root, criteria, reportData);
        return reportData;
    }

    static bool loadClientsFromFile(const QString& filename)
    {
        DebugLogger::log(QString("DataManager: Попытка загрузки клиентов из файла '%1'").arg(filename));

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            DebugLogger::log(QString("DataManager ERROR: Не удалось открыть файл с клиентами: '%1'").arg(filename));
            return false;
        }

        clients_array.clear();
        // clients_table.clear(); // Предполагается, что ваша HashTable имеет метод clear или пересоздается

        QTextStream in(&file);
        int index = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(';');

            if (parts.size() == 3 && validator.validateINN(parts[0].trimmed()) && validator.validateFIO(parts[1].trimmed()) && validator.validatePhone(parts[2].trimmed())) {
                Client client;
                client.inn = parts[0].trimmed().toULongLong();
                client.phone = parts[2].trimmed().toULongLong();
                client.fio = FIO(parts[1].trimmed());
                clients_array.append(client);
                clients_table.add(client.inn, index);
                index++;
            }
            else {
                qWarning() << "Incorrect line format in clients file:" << line;
            }
        }
        file.close();
        DebugLogger::log(QString("DataManager: Загружено %1 клиентов.").arg(clients_array.size()));
        return true;
    }

    static bool loadConsultationsFromFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }

        consultations_array.clear();
        consultations_tree.freeTree(consultations_tree.root);
        consultations_tree.root = nullptr;

        QTextStream in(&file);
        int index = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(';');

            if (parts.size() == 4 && validator.validateINN(parts[0].trimmed()) && validator.validateFIO(parts[2].trimmed()) && validator.validateDate(parts[3].trimmed())) {
                Consultation consultation;
                consultation.client_inn = parts[0].toULongLong();
                consultation.topic = parts[1].trimmed();
                consultation.lawyer_fio = FIO(parts[2].trimmed());
                consultation.date = Date(parts[3].trimmed());

                if (findClientByINN(consultation.client_inn) != nullptr) {
                    consultations_array.append(consultation);
                    consultations_tree.add(consultation.client_inn, index);
                    index++;
                }
                else {
                    qWarning() << "Не существует клиента с таким ИНН:" << line;
                }
            }
            else {
                qWarning() << "Некорректный формат строки в файле консультаций:" << line;
            }
        }
        file.close();
        qInfo() << "Загружено" << consultations_array.size() << "консультаций.";

        // Перестраиваем дерево фильтрации после загрузки данных
        rebuildFilterTree();
        return true;
    }

    static bool saveClientsToFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл для записи:" << filename;
            return false;
        }
        QTextStream out(&file);
        for (const Client& client : clients_array) {
            QString line = QString("%1;%2;%3")
                .arg(client.inn)
                .arg(client.fio.toString())
                .arg(QString::number(client.phone));
            out << line << "\n";
        }
        file.close();
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
        return true;
    }

    static bool addClient(const QString& inn, const QString& fio, const QString& phone) {
        DebugLogger::log(QString("DataManager: Попытка добавить клиента с ИНН %1").arg(inn));

        if (validator.validateINN(inn) && validator.validateFIO(fio) && validator.validatePhone(phone)) {
            quint64 validINN = inn.toULongLong();
            if (findClientByINN(validINN) != nullptr) {
                DebugLogger::log(QString("DataManager WARNING: Клиент с ИНН %1 уже существует.").arg(inn));
                return false;
            }
            clients_array.append({ validINN, FIO(fio), phone.toULongLong() });
            clients_table.add(validINN, clients_array.size() - 1);
            DebugLogger::log(QString("DataManager: Клиент с ИНН %1 успешно добавлен.").arg(inn));
            return true;
        }
        DebugLogger::log(QString("DataManager ERROR: Данные клиента с ИНН %1 не прошли валидацию.").arg(inn));
        return false;
    }

    static bool addConsultation(const QString& inn, const QString& fio, const QString& theme, const QString& date)
    {
        if (validator.validateINN(inn) && validator.validateFIO(fio) && validator.validateDate(date)) {
            quint64 validINN = inn.toULongLong();
            if (findClientByINN(validINN) == nullptr) {
                qWarning() << "Попытка добавить консультацию для несуществующего клиента с ИНН:" << validINN;
                return false;
            }

            int newIndex = consultations_array.size();
            Consultation newConsultation = { validINN, theme, FIO(fio), Date(date) };

            consultations_array.append(newConsultation);
            consultations_tree.add(validINN, newIndex);

            // --- ИЗМЕНЕНИЕ ---
            // Вместо полной перестройки, просто добавляем новый узел в дерево фильтрации
            quint64 dateKey = dateToKey(newConsultation.date);
            filter_tree_by_date.add(dateKey, newIndex);

            // rebuildFilterTree(); // УБИРАЕМ ЭТУ СТРОКУ

            return true;
        }
        return false;
    }

    static bool deleteClient(const QString& inn, const QString& fio, const QString& phone)
    {
        if (!validator.validateINN(inn) || !validator.validateFIO(fio) || !validator.validatePhone(phone)) return false;

        quint64 innToDelete_q64 = inn.toULongLong();
        const Client* clientToDelete = findClientByINN(innToDelete_q64);

        if (!clientToDelete || clientToDelete->fio.toString() != fio || clientToDelete->phone != phone.toULongLong()) {
            qInfo() << "Клиент не найден или данные не совпадают. Удаление отменено.";
            return false;
        }
        int indexToDeleteInClients = clientToDelete - &clients_array[0]; // Находим индекс клиента

        // --- Удаление связанных консультаций ---
        bool consultationsWereRemoved = false;
        QVector<int> indices = findConsultationIndicesByINN(innToDelete_q64);
        // Сортируем индексы в обратном порядке, чтобы удаление не сбивало последующие индексы
        std::sort(indices.rbegin(), indices.rend());
        for (int index : indices) {
            deleteConsultation(index);
            consultationsWereRemoved = true;
        }

        // Если консультации были удалены, дерево фильтрации уже перестроилось в deleteConsultation.
        // Если нет, ничего делать не нужно.

        // --- Удаление самого клиента (swap-and-pop) ---
        int lastClientIndex = clients_array.size() - 1;
        if (indexToDeleteInClients < lastClientIndex) {
            const Client& lastClient = clients_array.last();
            clients_array[indexToDeleteInClients] = lastClient;
            clients_table.updateIndex(lastClient.inn, indexToDeleteInClients);
        }
        clients_table.remove(innToDelete_q64);
        clients_array.removeLast();
        qInfo() << "Клиент с ИНН" << inn << "успешно удален.";
        return true;
    }

    //static bool deleteConsultation(int indexInArray)
    //{
    //    if (indexInArray < 0 || indexInArray >= consultations_array.size()) return false;

    //    const Consultation& toDelete = consultations_array.at(indexInArray);
    //    quint64 keyToDelete = toDelete.client_inn;
    //    int lastIndex = consultations_array.size() - 1;

    //    if (indexInArray < lastIndex) {
    //        const Consultation& lastElement = consultations_array.last();
    //        consultations_array[indexInArray] = lastElement;
    //        consultations_tree.remove(lastElement.client_inn, lastIndex);
    //        consultations_tree.add(lastElement.client_inn, indexInArray);
    //    }
    //    consultations_array.removeLast();
    //    consultations_tree.remove(keyToDelete, indexInArray);

    //    // Перестраиваем дерево фильтрации после удаления данных
    //    rebuildFilterTree();
    //    return true;
    //}
    static bool deleteConsultation(int indexInArray)
    {
        if (indexInArray < 0 || indexInArray >= consultations_array.size()) return false;

        // --- НАЧАЛО ИЗМЕНЕНИЙ ---
        // Шаг 1: Получаем информацию об удаляемом элементе ДО модификации массива
        const Consultation& toDelete = consultations_array.at(indexInArray);
        quint64 keyToDeleteFromMainTree = toDelete.client_inn;
        quint64 keyToDeleteFromFilterTree = dateToKey(toDelete.date);

        // Шаг 2: Удаляем запись из дерева фильтрации для удаляемого элемента
        filter_tree_by_date.remove(keyToDeleteFromFilterTree, indexInArray);

        // Шаг 3: Выполняем "swap-and-pop" и модифицируем основное дерево (consultations_tree)
        int lastIndex = consultations_array.size() - 1;
        if (indexInArray < lastIndex) {
            const Consultation& lastElement = consultations_array.last();
            consultations_array[indexInArray] = lastElement;

            // Обновляем основное дерево
            consultations_tree.remove(lastElement.client_inn, lastIndex);
            consultations_tree.add(lastElement.client_inn, indexInArray);

            // Шаг 4: Обновляем дерево фильтрации для перемещенного элемента
            quint64 lastElementDateKey = dateToKey(lastElement.date);
            // Сначала удаляем его старую запись по старому индексу
            filter_tree_by_date.remove(lastElementDateKey, lastIndex);
            // Затем добавляем новую запись с новым индексом
            filter_tree_by_date.add(lastElementDateKey, indexInArray);
        }

        consultations_array.removeLast();
        consultations_tree.remove(keyToDeleteFromMainTree, indexInArray);

        // rebuildFilterTree(); // УБИРАЕМ ЭТУ СТРОКУ
        // --- КОНЕЦ ИЗМЕНЕНИЙ ---
        return true;
    }

    static const QVector<Client>& getClients() { return clients_array; }
    static const QVector<Consultation>& getConsultations() { return consultations_array; }

    static const Client* findClientByINN(quint64 inn) {
        const Item* foundItem = clients_table.search(inn);
        if (foundItem) {
            int index = foundItem->index;
            if (index >= 0 && index < clients_array.size()) {
                return &clients_array[index];
            }
        }
        return nullptr;
    }

    static QVector<int> findConsultationIndicesByINN(quint64 inn) {
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