#pragma once
#include <HashTable.h>
#include <AVLTree.h>
#include <CustomVector.h>

#include <QString>
#include <stdexcept>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QDebug>

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

    inline static HashTable clients_table{ 10 };
    inline static CustomVector<Client> clients_array;

    inline static AVLTree<quint64> consultations_tree;
    inline static CustomVector<Consultation> consultations_array;

    inline static AVLTree<Date> filter_tree_by_date;
    //inline static AVLTree<FIO> filter_tree_by_client_fio;
    //inline static AVLTree<FIO> filter_tree_by_lawyer_fio;

    static void buildFilterTreeByDate(AVLTree<Date>& filter_tree) {
        filter_tree.freeTree(filter_tree.root);
        filter_tree.root = nullptr;

        for (int i = 0; i < consultations_array.size(); ++i) {
            const auto& consultation = consultations_array.at(i);
            filter_tree.add(consultation.date, i);
        }
    }
    //static void buildFilterTreeByClientFIO(AVLTree<FIO>& filter_tree) {
    //    filter_tree.freeTree(filter_tree.root);
    //    filter_tree.root = nullptr;

    //    for (int i = 0; i < clients_array.size(); ++i) {
    //        const auto& consultation = clients_array.at(i);
    //        filter_tree.add(consultation.fio, i);
    //    }
    //}
    //static void buildFilterTreeByLawyerFIO(AVLTree<FIO>& filter_tree) {
    //    filter_tree.freeTree(filter_tree.root);
    //    filter_tree.root = nullptr;

    //    for (int i = 0; i < consultations_array.size(); ++i) {
    //        const auto& consultation = consultations_array.at(i);
    //        filter_tree.add(consultation.lawyer_fio, i);
    //    }
    //}

    static void traverseForReport(TreeNode<Date>* node, const FilterCriteria& criteria, CustomVector<ReportEntry>& reportData) {
        if (!node) {
            return;
        }

        bool dateFilterActive = criteria.date.year > 0;

        if (dateFilterActive) {
            if (criteria.date < node->key) {
                traverseForReport(node->left, criteria, reportData);
            }
            else if (criteria.date > node->key) {
                traverseForReport(node->right, criteria, reportData);
            }
            else {
                ListNode* currentIndexNode = node->head;
                while (currentIndexNode) {
                    int consultationIndex = currentIndexNode->data;
                    if (consultationIndex < 0 || consultationIndex >= consultations_array.size()) {
                        currentIndexNode = currentIndexNode->next;
                        continue;
                    }
                    const Consultation& consultation = consultations_array.at(consultationIndex);
                    const Client* client = findClientByINN(consultation.client_inn);

                    if (client) {
                        bool clientFioMatch = criteria.client_fio.isEmpty() ||
                            (QString::compare(client->fio.toString(), criteria.client_fio, Qt::CaseInsensitive) == 0);

                        bool lawyerFioMatch = criteria.lawyer_fio.isEmpty() ||
                            (QString::compare(consultation.lawyer_fio.toString(), criteria.lawyer_fio, Qt::CaseInsensitive) == 0);

                        if (clientFioMatch && lawyerFioMatch) {
                            reportData.append({
                                client->inn, client->fio, client->phone,
                                consultation.topic, consultation.lawyer_fio, consultation.date
                                });
                        }
                    }
                    currentIndexNode = currentIndexNode->next;
                }
            }
        }
        else {
            traverseForReport(node->left, criteria, reportData);

            ListNode* currentIndexNode = node->head;
            while (currentIndexNode) {
                int consultationIndex = currentIndexNode->data;
                if (consultationIndex < 0 || consultationIndex >= consultations_array.size()) {
                    currentIndexNode = currentIndexNode->next;
                    continue;
                }
                const Consultation& consultation = consultations_array.at(consultationIndex);
                const Client* client = findClientByINN(consultation.client_inn);

                if (client) {
                    bool clientFioMatch = criteria.client_fio.isEmpty() ||
                        (QString::compare(client->fio.toString(), criteria.client_fio, Qt::CaseInsensitive) == 0);

                    bool lawyerFioMatch = criteria.lawyer_fio.isEmpty() ||
                        (QString::compare(consultation.lawyer_fio.toString(), criteria.lawyer_fio, Qt::CaseInsensitive) == 0);

                    if (clientFioMatch && lawyerFioMatch) {
                        reportData.append({
                            client->inn, client->fio, client->phone,
                            consultation.topic, consultation.lawyer_fio, consultation.date
                            });
                    }
                }
                currentIndexNode = currentIndexNode->next;
            }

            traverseForReport(node->right, criteria, reportData);
        }
    }

public:
    static CustomVector<ReportEntry> generateReport(const FilterCriteria& criteria) {
        CustomVector<ReportEntry> reportData;
        traverseForReport(filter_tree_by_date.root, criteria, reportData);
        return reportData;
    }

    static bool loadClientsFromFile(const QString& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл с клиентами:" << filename;
            return false;
        }

        clients_array.clear();

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
        qInfo() << "Загружено" << clients_array.size() << "клиентов.";
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

        buildFilterTreeByDate(filter_tree_by_date);
        //buildFilterTree(filter_tree_by_client_fio);
        //buildFilterTree(filter_tree_by_lawyer_fio);
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
        if (validator.validateINN(inn) && validator.validateFIO(fio) && validator.validatePhone(phone)) {
            quint64 validINN = inn.toULongLong();
            if (findClientByINN(validINN) != nullptr) {
                qWarning() << "Клиент с ИНН" << inn << "уже существует.";
                return false;
            }
            clients_array.append({ validINN, FIO(fio), phone.toULongLong() });
            clients_table.add(validINN, clients_array.size() - 1);
            return true;
        }
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
            consultations_array.append({ validINN, theme, FIO(fio), Date(date) });

            // Добавляем в деревья фильтрации
            const auto& newConsultation = consultations_array.last();
            filter_tree_by_date.add(newConsultation.date, newIndex);

            consultations_tree.add(validINN, newIndex);
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
            qInfo() << "Клиент с ИНН" << inn << "не найден. Удаление отменено.";
            return false;
        }

        // Сначала удаляем все связанные консультации
        CustomVector<int> indicesToDelete = findConsultationIndicesByINN(innToDelete_q64);
        // Важно: сортируем индексы в обратном порядке, чтобы не сдвигать оставшиеся
        std::sort(indicesToDelete.rbegin(), indicesToDelete.rend());
        for (int index : indicesToDelete) {
            deleteConsultation(index);
        }

        // Теперь удаляем самого клиента
        int indexToDeleteInClients = clientToDelete - &clients_array[0];
        int lastClientIndex = clients_array.size() - 1;
        if (indexToDeleteInClients < lastClientIndex) {
            const Client& lastClient = clients_array.last();
            clients_array[indexToDeleteInClients] = lastClient;
            clients_table.updateIndex(lastClient.inn, indexToDeleteInClients);
        }
        clients_table.remove(innToDelete_q64);
        clients_array.removeLast();

        qInfo() << "Клиент с ИНН" << inn << "и все его консультации успешно удалены.";
        return true;
    }

    static bool deleteConsultation(int indexInArray)
    {
        if (indexInArray < 0 || indexInArray >= consultations_array.size()) return false;

        const Consultation& toDelete = consultations_array.at(indexInArray);
        quint64 innToDelete = toDelete.client_inn;
        Date dateKeyToDelete = toDelete.date;

        int lastIndex = consultations_array.size() - 1;

        if (indexInArray < lastIndex) {
            const Consultation& lastElement = consultations_array.last();
            consultations_array[indexInArray] = lastElement;

            consultations_tree.remove(lastElement.client_inn, lastIndex);
            filter_tree_by_date.remove(lastElement.date, lastIndex);

            consultations_tree.add(lastElement.client_inn, indexInArray);
            filter_tree_by_date.add(lastElement.date, indexInArray);
        }

        consultations_array.removeLast();

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

        for (int i = 0; i < consultations_array.size(); ++i) {
            const auto& consultation = consultations_array.at(i);
            if (consultation.client_inn == inn &&
                consultation.lawyer_fio.toString() == lawyerFio &&
                consultation.topic == topic &&
                consultation.date == d)
            {
                // Нашли совпадение, вызываем существующую функцию удаления по индексу
                return deleteConsultation(i);
            }
        }

        // Если прошли весь цикл и не нашли совпадения
        qWarning() << "Консультация с указанными параметрами не найдена.";
        return false;
    }

    static const CustomVector<Client>& getClients() { return clients_array; }
    static const CustomVector<Consultation>& getConsultations() { return consultations_array; }

    static const Client* findClientByINN(quint64 inn, int& steps) {
        const Item* foundItem = clients_table.search(inn, steps);
        if (foundItem) {
            int index = foundItem->index;
            if (index >= 0 && index < clients_array.size()) {
                return &clients_array[index];
            }
        }
        return nullptr;
    }

    static const Client* findClientByINN(quint64 inn) {
        int dummy_steps = 0;
        return findClientByINN(inn, dummy_steps);
    }

    static CustomVector<int> findConsultationIndicesByINN(quint64 inn, int& steps) {
        CustomVector<int> indices;
        TreeNode<quint64>* node = consultations_tree.find(inn, steps);
        if (node) {
            ListNode* current = node->head;
            while (current) {
                indices.append(current->data);
                current = current->next;
            }
        }
        return indices;
    }

    static CustomVector<int> findConsultationIndicesByINN(quint64 inn) {
        int dummy_steps = 0;
        return findConsultationIndicesByINN(inn, dummy_steps);
    }

    // ОТЛАДКА
    static QString getClientsTableState() { return QString::fromStdString(clients_table.toString()); }
    static QString getConsultationsTreeState() { return QString::fromStdString(consultations_tree.toString(consultations_tree.root)); }
    static QString getFilterTreeByDateState() { return  QString::fromStdString(filter_tree_by_date.toString(filter_tree_by_date.root)); }
};