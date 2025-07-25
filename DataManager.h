﻿#pragma once
#include <HashTable.h>
#include <AVLTree.h>

#include <QRegularExpression> //для проверок на валидность
#include <QFile> //файловый ввод-вывод

// Константа для размера массивов
#define MAX_RECORDS 10000

struct Date {
    Date() : day(0), month(0), year(0) {}

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

    bool operator<(const Date& other) const {
        if (year < other.year) return true;
        if (year > other.year) return false;

        if (month < other.month) return true;
        if (month > other.month) return false;

        return day < other.day;
    }

    bool operator==(const Date& other) const {
        return (year == other.year && month == other.month && day == other.day);
    }

    bool operator>(const Date& other) const {
        return other < *this;
    }
};

inline ostream& operator<<(std::ostream& os, const Date& date) {
    os << date.toString().toStdString();
    return os;
}

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

    QString toFileLine() const {
        return QString("%1;%2;%3")
            .arg(inn)
            .arg(fio.toString())
            .arg(QString::number(phone));
    }
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
    quint64 client_inn;
    FIO client_fio;
    quint64 phone;
    QString topic;
    FIO lawyer_fio;
    Date date;
};

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

public:
    inline static Client clients_array[MAX_RECORDS];
    inline static int clients_array_size = 0;
    inline static HashTable clients_table{ 17 };

    inline static Consultation consultations_array[MAX_RECORDS];
    inline static int consultations_array_size = 0;
    inline static AVLTree<quint64> consultations_tree;

    inline static AVLTree<Date> filter_tree_by_date;

private:
    static void buildFilterTreeByDate(AVLTree<Date>& filter_tree) {
        filter_tree.freeTree(filter_tree.root);
        filter_tree.root = nullptr;

        for (int i = 0; i < consultations_array_size; ++i) {
            const auto& consultation = consultations_array[i];
            filter_tree.add(consultation.date, i);
        }
    }


    static void traverseForReport(TreeNode<Date>* node, const FilterCriteria& criteria, ReportEntry* reportData, int& reportDataSize, int maxReportSize) {
        if (!node) {
            return;
        }

        bool dateFilterActive = criteria.date.year > 0;

        if (dateFilterActive) {
            if (criteria.date < node->key) {
                traverseForReport(node->left, criteria, reportData, reportDataSize, maxReportSize);
            }
            else if (criteria.date > node->key) {
                traverseForReport(node->right, criteria, reportData, reportDataSize, maxReportSize);
            }
            else {
                ListNode* currentIndexNode = node->head;
                while (currentIndexNode) {
                    int consultationIndex = currentIndexNode->data;
                    if (consultationIndex < 0 || consultationIndex >= consultations_array_size) {
                        currentIndexNode = currentIndexNode->next;
                        continue;
                    }
                    const Consultation& consultation = consultations_array[consultationIndex];
                    const Client* client = findClientByINN(consultation.client_inn);

                    if (client) {
                        bool clientFioMatch = criteria.client_fio.isEmpty() ||
                            (QString::compare(client->fio.toString(), criteria.client_fio, Qt::CaseInsensitive) == 0);

                        bool lawyerFioMatch = criteria.lawyer_fio.isEmpty() ||
                            (QString::compare(consultation.lawyer_fio.toString(), criteria.lawyer_fio, Qt::CaseInsensitive) == 0);

                        if (clientFioMatch && lawyerFioMatch) {
                            if (reportDataSize < maxReportSize) {
                                reportData[reportDataSize++] = {
                                    client->inn, client->fio, client->phone,
                                    consultation.topic, consultation.lawyer_fio, consultation.date
                                };
                            }
                        }
                    }
                    currentIndexNode = currentIndexNode->next;
                }
            }
        }
        else {
            traverseForReport(node->left, criteria, reportData, reportDataSize, maxReportSize);

            ListNode* currentIndexNode = node->head;
            while (currentIndexNode) {
                int consultationIndex = currentIndexNode->data;
                if (consultationIndex < 0 || consultationIndex >= consultations_array_size) {
                    currentIndexNode = currentIndexNode->next;
                    continue;
                }
                const Consultation& consultation = consultations_array[consultationIndex];
                const Client* client = findClientByINN(consultation.client_inn);

                if (client) {
                    bool clientFioMatch = criteria.client_fio.isEmpty() ||
                        (QString::compare(client->fio.toString(), criteria.client_fio, Qt::CaseInsensitive) == 0);

                    bool lawyerFioMatch = criteria.lawyer_fio.isEmpty() ||
                        (QString::compare(consultation.lawyer_fio.toString(), criteria.lawyer_fio, Qt::CaseInsensitive) == 0);

                    if (clientFioMatch && lawyerFioMatch) {
                        if (reportDataSize < maxReportSize) {
                            reportData[reportDataSize++] = {
                                client->inn, client->fio, client->phone,
                                consultation.topic, consultation.lawyer_fio, consultation.date
                            };
                        }
                    }
                }
                currentIndexNode = currentIndexNode->next;
            }

            traverseForReport(node->right, criteria, reportData, reportDataSize, maxReportSize);
        }
    }
public:
    // ИЗМЕНЕНО: Функция теперь принимает указатель на массив для результатов и возвращает количество записей.
    static int generateReport(const FilterCriteria& criteria, ReportEntry* reportDataArray, int maxReportSize) {
        int reportDataSize = 0;
        traverseForReport(filter_tree_by_date.root, criteria, reportDataArray, reportDataSize, maxReportSize);
        return reportDataSize;
    }

    static bool loadClientsFromFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }

        clients_array_size = 0;

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(';');

            if (parts.size() == 3 && validator.validateINN(parts[0].trimmed()) && validator.validateFIO(parts[1].trimmed()) && validator.validatePhone(parts[2].trimmed())) {
                Client client;
                client.inn = parts[0].trimmed().toULongLong();
                client.phone = parts[2].trimmed().toULongLong();
                client.fio = FIO(parts[1].trimmed());

                if (clients_array_size < MAX_RECORDS) {
                    clients_array[clients_array_size] = client;
                    clients_table.add(client.inn, clients_array_size);
                    clients_array_size++;
                }
                else {
                    break;
                }
            }
        }
        file.close();
        return true;
    }

    static bool loadConsultationsFromFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }

        consultations_array_size = 0;
        consultations_tree.freeTree(consultations_tree.root);
        consultations_tree.root = nullptr;

        QTextStream in(&file);
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
                    if (consultations_array_size < MAX_RECORDS) {
                        consultations_array[consultations_array_size] = consultation;
                        consultations_tree.add(consultation.client_inn, consultations_array_size);
                        consultations_array_size++;
                    }
                    else {
                        break;
                    }
                }
            }
        }
        file.close();

        buildFilterTreeByDate(filter_tree_by_date);
        return true;
    }

    static bool saveClientsToFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream out(&file);
        for (int i = 0; i < clients_array_size; ++i) {
            out << clients_array[i].toFileLine() << "\n";
        }
        file.close();
        return true;
    }

    static bool saveConsultationsToFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream out(&file);
        for (int i = 0; i < consultations_array_size; ++i) {
            out << consultations_array[i].toFileLine() << "\n";
        }
        file.close();
        return true;
    }

    static bool addClient(const QString& inn, const QString& fio, const QString& phone) {
        if (validator.validateINN(inn) && validator.validateFIO(fio) && validator.validatePhone(phone)) {
            quint64 validINN = inn.toULongLong();
            if (findClientByINN(validINN) != nullptr) {
                return false;
            }
            if (clients_array_size >= MAX_RECORDS) {
                return false;
            }
            clients_array[clients_array_size] = { validINN, FIO(fio), phone.toULongLong() };
            clients_table.add(validINN, clients_array_size);
            clients_array_size++;
            return true;
        }
        return false;
    }

    static bool addConsultation(const QString& inn, const QString& fio, const QString& theme, const QString& date)
    {
        if (validator.validateINN(inn) && validator.validateFIO(fio) && validator.validateDate(date)) {
            quint64 validINN = inn.toULongLong();
            if (findClientByINN(validINN) == nullptr) {
                return false;
            }
            if (consultations_array_size >= MAX_RECORDS) {
                return false;
            }

            int newIndex = consultations_array_size;
            consultations_array[newIndex] = { validINN, theme, FIO(fio), Date(date) };

            const auto& newConsultation = consultations_array[newIndex];
            filter_tree_by_date.add(newConsultation.date, newIndex);
            consultations_tree.add(validINN, newIndex);

            consultations_array_size++;
            return true;
        }
        return false;
    }

    static bool deleteClientByINN(const QString& inn)
    {
        if (!validator.validateINN(inn)) return false;

        quint64 innToDelete_q64 = inn.toULongLong();
        const Client* clientToDelete = findClientByINN(innToDelete_q64);

        if (!clientToDelete) {
            return false;
        }

        // ИЗМЕНЕНО: Используем статический массив вместо QVector
        int indicesToDelete[MAX_RECORDS];
        int numIndicesToDelete = findConsultationIndicesByINN(innToDelete_q64, indicesToDelete, MAX_RECORDS);

        // ИЗМЕНЕНО: Сортировка массива с помощью std::sort
        std::sort(indicesToDelete, indicesToDelete + numIndicesToDelete, std::greater<int>());

        for (int i = 0; i < numIndicesToDelete; ++i) {
            deleteConsultation(indicesToDelete[i]);
        }

        int indexToDeleteInClients = clientToDelete - &clients_array[0];
        int lastClientIndex = clients_array_size - 1;

        clients_table.remove(innToDelete_q64);

        if (indexToDeleteInClients < lastClientIndex) {
            const Client& lastClient = clients_array[lastClientIndex];
            clients_array[indexToDeleteInClients] = lastClient;
            clients_table.remove(lastClient.inn);
            clients_table.add(lastClient.inn, indexToDeleteInClients);
        }
        if (clients_array_size > 0) {
            clients_array_size--;
        }

        return true;
    }

    static bool deleteConsultation(int indexInArray)
    {
        if (indexInArray < 0 || indexInArray >= consultations_array_size) return false;

        const Consultation& toDelete = consultations_array[indexInArray];
        quint64 innToDelete = toDelete.client_inn;
        Date dateKeyToDelete = toDelete.date;

        int lastIndex = consultations_array_size - 1;

        if (indexInArray < lastIndex) {
            const Consultation& lastElement = consultations_array[lastIndex];
            consultations_array[indexInArray] = lastElement;

            consultations_tree.remove(lastElement.client_inn, lastIndex);
            filter_tree_by_date.remove(lastElement.date, lastIndex);

            consultations_tree.add(lastElement.client_inn, indexInArray);
            filter_tree_by_date.add(lastElement.date, indexInArray);
        }

        if (consultations_array_size > 0) {
            consultations_array_size--;
        }

        consultations_tree.remove(innToDelete, indexInArray);
        filter_tree_by_date.remove(dateKeyToDelete, indexInArray);

        return true;
    }

    static bool deleteConsultationByMatch(const QString& clientInn, const QString& lawyerFio, const QString& topic, const QString& date)
    {
        if (!validator.validateINN(clientInn) || !validator.validateFIO(lawyerFio) || !validator.validateDate(date)) {
            return false;
        }

        quint64 inn = clientInn.toULongLong();
        Date d(date);

        for (int i = 0; i < consultations_array_size; ++i) {
            const auto& consultation = consultations_array[i];
            if (consultation.client_inn == inn &&
                consultation.lawyer_fio.toString() == lawyerFio &&
                consultation.topic == topic &&
                consultation.date == d)
            {
                return deleteConsultation(i);
            }
        }

        return false;
    }

    static const Client* findClientByINN(quint64 inn, int& steps) {
        const Item* foundItem = clients_table.search(inn, steps);
        if (foundItem) {
            int index = foundItem->index;
            if (index >= 0 && index < clients_array_size) {
                return &clients_array[index];
            }
        }
        return nullptr;
    }

    static const Client* findClientByINN(quint64 inn) {
        int dummy_steps = 0;
        return findClientByINN(inn, dummy_steps);
    }

    static int findConsultationIndicesByINN(quint64 inn, int* out_indices, int max_size, int& steps) {
        int count = 0;
        TreeNode<quint64>* node = consultations_tree.find(inn, steps);
        if (node) {
            ListNode* current = node->head;
            while (current) {
                if (count < max_size) {
                    out_indices[count++] = current->data;
                }
                else {
                    break;
                }
                current = current->next;
            }
        }
        return count;
    }

    static int findConsultationIndicesByINN(quint64 inn, int* out_indices, int max_size) {
        int dummy_steps = 0;
        return findConsultationIndicesByINN(inn, out_indices, max_size, dummy_steps);
    }

    static QString getClientsTableState() { return QString::fromStdString(clients_table.toString()); }
    static QString getConsultationsTreeState() { return QString::fromStdString(consultations_tree.toString(consultations_tree.root)); }
    static QString getFilterTreeByDateState() { return  QString::fromStdString(filter_tree_by_date.toString(filter_tree_by_date.root)); }
};