#include <iostream>
#include <string>
#include <clocale>
using namespace std;

struct Node {
    int data;
    Node* next;
};

class List {
public:
    //1. Инициализация
    Node* init() {
        return nullptr;
    }

    //2. Освобождение памяти
    void dispose(Node*& head) {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

    //3. Добавление элемента (упорядоченный по возрастанию)
    void addSorted(Node*& head, int value) {
        Node* newNode = new Node{ value, nullptr };

        if (!head || value < head->data) {
            newNode->next = head;
            head = newNode;
            return;
        }

        Node* current = head;
        while (current->next && current->next->data < value) {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    //4. Удаление всех вхождений заданного по значению элемента
    void removeAll(Node*& head, int value) {
        while (head && head->data == value) {
            Node* temp = head;
            head = temp->next;
            delete temp;
            temp = nullptr;
        }

        Node* current = head;
        while (current && current->next) {
            if (current->next->data == value) {
                Node* temp = current->next;
                current->next = temp->next;
                delete temp;
                temp = nullptr;
            }
            else {
                current = current->next;
            }
        }
    }

    //5. Удаление (после каждого вхождения заданного)
    void removeAfter(Node*& head, int value) {
        Node* current = head;
        while (current && current->next) {
            if (current->data == value) {
                Node* temp = current->next;
                current->next = temp->next;
                delete temp;
                temp = nullptr;
            }
            current = current->next;
        }
    }

    //6. Поиск заданного элемента по значению
    bool find(Node* head, int value) {
        Node* current = head;
        while (current) {
            if (current->data == value)
                return true;
            current = current->next;
        }
        return false;
    }

    //7. Печать
    void print(Node* head) {
        Node* current = head;
        while (current) {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }

    //8. Операция работы с двумя списками (симметрическая разность)
    Node* symmetricDifference(Node* fst_head, Node* scnd_head) {
        Node* result = nullptr;

        Node* current1 = fst_head;
        Node* current2 = scnd_head;

        int value;

        while (current1 && current2) {
            if (current1->data < current2->data) { // fst <
                value = current1->data;
                addSorted(result, value);
                while (current1 && current1->data == value) {
                    current1 = current1->next;
                }
            }
            else if (current1->data > current2->data) { // fst >
                value = current2->data;
                addSorted(result, value);
                while (current2 && current2->data == value) {
                    current2 = current2->next;
                }
            }
            else { // fst =
                int value = current1->data;
                while (current1 && current1->data == value) {
                    current1 = current1->next;
                }
                while (current1 && current2->data == value) {
                    current2 = current2->next;
                }
            }
        }

        while (current1) {
            addSorted(result, current1->data);
            current1 = current1->next;
        }

        while (current2) {
            addSorted(result, current2->data);
            current2 = current2->next;
        }

        return result;
    }

    //9 (10). «Перевернуть» заданный список
    void reverse(Node*& head) {
        Node* prev = nullptr;
        Node* current = head;
        Node* next = nullptr;

        while (current != nullptr) {
            next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }
        head = prev;
    }
};

int main()
{
    setlocale(LC_ALL, "RU");

    List list;
    int choice, value;
    bool is_fst_active = true;

    Node* fst_head = list.init();
    Node* scnd_head = list.init();
    Node** curr_head = &fst_head;

    do {
        cout << ((is_fst_active) ? "Активный список: 1" : "Активный список: 2") << "\n";
        cout << "Меню:\n";
        cout << "1. Инициализация списка\n";
        cout << "2. Освобождение памяти\n";
        cout << "3. Добавление элемента\n";
        cout << "4. Удаление всех вхождений элемента\n";
        cout << "5. Удаление после каждого вхождения\n";
        cout << "6. Поиск элемента\n";
        cout << "7. Печать списка\n";
        cout << "8. Симметрическая разность двух списков\n";
        cout << "9. Изменить активный список\n";
        cout << "10. «Перевернуть» список\n";
        cout << "0. Выход\n";
        cout << "Выберите действие: ";
        cin >> choice;

        switch (choice) {
        case 1: //+1. Инициализация
            *curr_head = list.init();
            cout << "Список инициализирован.\n";
            break;
        case 2: //+2. Освобождение памяти
            list.dispose(*curr_head);
            cout << "Память освобождена.\n";
            break;
        case 3: //+3. Добавление элемента (упорядоченный по возрастанию)
            cout << "Введите значение для добавления: ";
            cin >> value;
            list.addSorted(*curr_head, value);
            cout << "Элемент добавлен.\n";
            break;
        case 4: //+4. Удаление всех вхождений заданного по значению элемента
            cout << "Введите значение для удаления: ";
            cin >> value;
            list.removeAll(*curr_head, value);
            cout << "Вхождения удалены.\n";
            break;
        case 5: //+5. Удаление (после каждого вхождения заданного)
            cout << "Введите значение, после каждого вхождения которого необходимо производить удаление: ";
            cin >> value;
            list.removeAfter(*curr_head, value);
            break;
        case 6: //+6. Поиск заданного элемента по значению
            cout << "Введите значение для поиска: ";
            cin >> value;
            if (list.find(*curr_head, value)) {
                cout << "Элемент найден.\n";
            }
            else cout << "Элемент не найден.\n";
            break;
        case 7: //+7. Печать
            cout << "Список: ";
            list.print(*curr_head);
            break;
        case 8: //+8. Операция работы с двумя списками (симметрическая разность)
            cout << "Первый список: ";
            list.print(fst_head);
            cout << "Второй список: ";
            list.print(scnd_head);

            cout << "Результат: ";
            list.print(list.symmetricDifference(fst_head, scnd_head));
            break;
        case 9:
            if (!is_fst_active) {
                curr_head = &fst_head;
            }
            else {
                curr_head = &scnd_head;
            }
            is_fst_active = !is_fst_active;
            cout << "Активный список изменен.\n";
            break;
        case 10:
            list.reverse(*curr_head);
            cout << "Список перевернут.\n";
            break;
        case 0:
            cout << "Выход из программы.\n";
            break;
        default:
            cout << "Некорректный выбор.\n";
        }
        cout << "\n";
    } while (choice != 0);

    list.dispose(fst_head);
    list.dispose(scnd_head);
    return 0;
}