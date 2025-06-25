#pragma once
#include <HashTable.h>

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
        //QFile file(filename);
        //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //    qWarning() << "Failed to open consultations file:" << filename; // Original: "Не удалось открыть файл с консультациями:"
        //    return false;
        //}

        //consultations_array.clear();

        //QTextStream in(&file);

        //while (!in.atEnd()) {
        //    QString line = in.readLine();
        //    QStringList parts = line.split(';');

        //    if (parts.size() == 4) {
        //        Consultation consultation;
        //        consultation.client_inn = parts[0].trimmed();
        //        consultation.topic = parts[1].trimmed();

        //        QStringList lawyerFioParts = parts[2].trimmed().split(' ');
        //        if (lawyerFioParts.size() == 3) {
        //            consultation.lawyer_fio.f = lawyerFioParts[0];
        //            consultation.lawyer_fio.i = lawyerFioParts[1];
        //            consultation.lawyer_fio.o = lawyerFioParts[2];
        //        }
        //        else {
        //            qWarning() << "Incorrect lawyer FIO format in consultations file line:" << line;
        //            continue; // Skip this line if FIO is malformed
        //        }


        //        QString dateString = parts[3].trimmed();
        //        QStringList dateParts = dateString.split('.');
        //        if (dateParts.size() == 3) {
        //            consultation.date.day = dateParts[0].toInt();
        //            consultation.date.month = dateParts[1].toInt();
        //            consultation.date.year = dateParts[2].toInt();

        //            try {
        //                Validator::validateDate(consultation.date.day, consultation.date.month, consultation.date.year);
        //                consultations_array.append(consultation);
        //            }
        //            catch (const std::invalid_argument& e) {
        //                qWarning() << "Invalid date in consultations file line:" << line << "Error:" << e.what();
        //            }
        //        }
        //        else {
        //            qWarning() << "Incorrect date format in consultations file line:" << line;
        //        }
        //    }
        //    else {
        //        qWarning() << "Incorrect line format in consultations file:" << line; // Original: "Некорректный формат строки в файле консультаций:"
        //    }
        //}

        //file.close();
        //return true;
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

    static const QVector<Client>& getClients() 
    {
        return clients_array;
    }

    const QVector<Consultation>& getConsultations() const
    {
        return consultations_array;
    }

    static const Client* findClientByINN(quint64 inn)
    {
        // 1. Ищем в хеш-таблице. Она возвращает индекс в массиве clients_array.
        int index = clients_table.search(inn)->index;

        // 2. Если индекс валидный (не -1), возвращаем указатель на клиента в массиве.
        if (index != -1 && index < clients_array.size()) {
            return &clients_array[index];
        }

        // 3. Если ничего не найдено, возвращаем nullptr.
        return nullptr;
    }
};