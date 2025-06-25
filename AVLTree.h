#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <regex>

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
            cout << current->data;
            if (current->next) cout << ", ";
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

    //для печати списка
    string toString(Node* head) const {
        string result = "";
        Node* current = head;
        while (current) {
            result += to_string(current->data);
            if (current->next) {
                result += ", ";
            }
            current = current->next;
        }
        return result;
    }
};

struct PersonName {
    string lastName;
    string firstName;
    string middleName;

    bool operator<(const PersonName& other) const {
        if (lastName != other.lastName) return lastName < other.lastName;
        if (firstName != other.firstName) return firstName < other.firstName;
        return middleName < other.middleName;
    }

    bool operator>(const PersonName& other) const {
        if (lastName != other.lastName) return lastName > other.lastName;
        if (firstName != other.firstName) return firstName > other.firstName;
        return middleName > other.middleName;
    }

    bool operator==(const PersonName& other) const {
        return lastName == other.lastName && firstName == other.firstName && middleName == other.middleName;
    }

    bool operator!=(const PersonName& other) const {
        return !(lastName == other.lastName && firstName == other.firstName && middleName == other.middleName);
    }
};

//структура для представления ключа (ФИО + ФИО)
struct Key {
    PersonName person1;
    PersonName person2;

    string toString() const {
        return person1.lastName + " " + person1.firstName + " " + person1.middleName + " " +
            person2.lastName + " " + person2.firstName + " " + person2.middleName;
    }

    bool operator<(const Key& other) const {
        if (person1 != other.person1) return person1 < other.person1;
        return person2 < other.person2;
    }

    bool operator>(const Key& other) const {
        if (person1 != other.person1) return person1 > other.person1;
        return person2 > other.person2;
    }

    bool operator==(const Key& other) const {
        return person1 == other.person1 && person2 == other.person2;
    }
};

struct TreeNode {
    Key key;
    List lineNumbers;
    Node* head;
    int balanceFactor;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Key& k, int line) : key(k), balanceFactor(0), left(nullptr), right(nullptr), head(nullptr) {
        lineNumbers.addSorted(head, line);
    }
};

struct AVLTree {
    TreeNode* root;

    AVLTree() : root(nullptr) {}
};

//проверка корректности ФИО
bool isValidFullName(const string& fullName) {
    regex fullNameRegex(R"(^[А-ЯЁ][а-яё]+ [А-ЯЁ][а-яё]+ [А-ЯЁ][а-яё]+$)");

    return regex_match(fullName, fullNameRegex);
}

//3. Удаление заданного элемента (максимальный слева)
TreeNode* balanceL(TreeNode* p, bool& h) {
    TreeNode* p1;
    TreeNode* p2;
    if (p->balanceFactor == -1) {
        p->balanceFactor = 0;
    }
    else if (p->balanceFactor == 0) {
        p->balanceFactor = 1;
        h = false;
    }
    else /*(p->balanceFactor == 1)*/ {
        p1 = p->right;
        if (p1->balanceFactor >= 0) { //Одиночная RR
            p->right = p1->left;
            p1->left = p;
            if (p1->balanceFactor == 0) {
                p->balanceFactor = 1;
                p1->balanceFactor = -1;
                h = false;
            }
            else {
                p->balanceFactor = 0;
                p1->balanceFactor = 0;
            }
            p = p1;
        }
        else { //двойная RL
            p2 = p1->left;
            p1->left = p2->right;
            p2->right = p1;
            p->right = p2->left;
            p2->left = p;
            if (p2->balanceFactor == 1) p->balanceFactor = -1;
            else p->balanceFactor = 0;
            if (p2->balanceFactor == -1) p1->balanceFactor = 1;
            else p1->balanceFactor = 0;
            p = p2;
            p2->balanceFactor = 0;
        }
        h = false;
    }
    return p;
}

TreeNode* balanceR(TreeNode* p, bool& h) {
    TreeNode* p1;
    TreeNode* p2;

    if (p->balanceFactor == 1) {
        p->balanceFactor = 0;
    }
    else if (p->balanceFactor == 0) {
        p->balanceFactor = -1;
        h = false;
    }
    else /*p->balanceFactor = -1, rebalance*/ {
        p1 = p->left;
        if (p1->balanceFactor <= 0) { //одиночная LL
            p->left = p1->right;
            p1->right = p;
            if (p1->balanceFactor == 0) {
                p->balanceFactor = -1;
                p1->balanceFactor = 1;
                h = false;
            }
            else {
                p->balanceFactor = 0;
                p1->balanceFactor = 0;
            }
            p = p1;
        }
        else { //двойная LR
            p2 = p1->right;
            p1->right = p2->left;
            p2->left = p1;
            p->left = p2->right;
            p2->right = p;
            if (p2->balanceFactor == -1) p->balanceFactor = 1;
            else p->balanceFactor = 0;
            if (p2->balanceFactor == 1) p1->balanceFactor = -1;
            else p1->balanceFactor = 0;
            p = p2;
            p2->balanceFactor = 0;
        }
        h = false;
    }
    return p;
}


TreeNode* del(TreeNode* r, TreeNode* q, bool& h) {
    if (r->right != nullptr) {
        r->right = del(r->right, q, h);
        if (h) r = balanceR(r, h);
    }
    else {
        q->key = r->key;
        //копируем список номеров строк
        q->lineNumbers.dispose(q->head); //очищаем старый список
        Node* current = r->head;
        while (current) {
            q->lineNumbers.addSorted(q->head, current->data);
            current = current->next;
        }

        q = r;
        r = r->left;
        h = true;
    }
    return r;
}

//3. Удаление заданного элемента (максимальный слева) (при полном совпадении ключа и номера строки)
TreeNode* deleteNode(TreeNode* p, const Key& key, int lineNumber, bool& h) {
    TreeNode* q;

    if (p == nullptr) {
        cout << "Элемент не найден" << endl;
    }
    else if (p->key > key) {
        p->left = deleteNode(p->left, key, lineNumber, h);
        if (h) p = balanceL(p, h);
    }
    else if (p->key < key) {
        p->right = deleteNode(p->right, key, lineNumber, h);
        if (h) p = balanceR(p, h);
    }
    else { //ключ найден
        if (p->lineNumbers.find(p->head, lineNumber)) {
            p->lineNumbers.removeAll(p->head, lineNumber);

            if (p->head == nullptr) {
                q = p;
                if (q->right == nullptr) {
                    p = q->left;
                    delete q;
                    h = true;
                }
                else if (q->left == nullptr) {
                    p = q->right;
                    delete q;
                    h = true;
                }
                else {
                    q->left = del(q->left, q, h); //поиск max в левом поддереве
                    if (h) p = balanceL(p, h);
                }
            }
            else {
                h = false;
            }
        }
        else {
            cout << "Номер строки не найден для данного ключа" << endl;
            h = false;
        }
    }
    return p;
}

//2. Добавление нового элемента
TreeNode* insert(TreeNode* p, const Key& key, int lineNumber, bool& h) {
    TreeNode* p1;
    TreeNode* p2;

    if (p == nullptr) {
        p = new TreeNode(key, lineNumber);
        h = true;
    }
    else if (p->key > key) {
        p->left = insert(p->left, key, lineNumber, h);
        if (h) {
            switch (p->balanceFactor) {
            case 0:
                p->balanceFactor = -1;
                break;
            case 1:
                p->balanceFactor = 0;
                h = false;
                break;
            case -1:
                return balanceR(p, h);
            }
        }
    }
    else if (p->key < key) {
        p->right = insert(p->right, key, lineNumber, h);
        if (h) {
            switch (p->balanceFactor) {
            case 0:
                p->balanceFactor = 1;
                break;
            case -1:
                p->balanceFactor = 0;
                h = false;
                break;
            case 1:
                return balanceL(p, h);
            }
        }
    }
    else {
        p->lineNumbers.addSorted(p->head, lineNumber);
        h = false;
    }
    return p;
}

//4. Поиск заданного элемента
TreeNode* search(TreeNode* node, const Key& key) {
    if (node == nullptr || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return search(node->left, key);
    }

    return search(node->right, key);
}

//5. Печать
void preOrderPrint(TreeNode* node, ofstream& outputFile) {
    if (node) {
        outputFile << node->key.toString() << " ";
        outputFile << node->lineNumbers.toString(node->head) << endl;
        preOrderPrint(node->left, outputFile);
        preOrderPrint(node->right, outputFile);
    }
}

void printToFile(const AVLTree& tree, const string& filename) {
    ofstream outputFile(filename);
    if (outputFile.is_open()) {
        preOrderPrint(tree.root, outputFile);
        outputFile.close();
    }
    else {
        cerr << "Ошибка в открытии файла: " << filename << endl;
    }
}

//6. Обход: прямой
void printToScreen(TreeNode* node, int indent = 0) {
    if (node != nullptr) {
        if (node->right) {
            printToScreen(node->right, indent + 4);
        }
        if (indent) {
            cout << setw(indent) << ' ';
        }
        if (node->right) cout << " /\n" << setw(indent) << ' ';
        cout << node->key.toString() << " (" << node->lineNumbers.toString(node->head) << ")" << "\n ";
        if (node->left) {
            cout << setw(indent) << ' ' << " \\\n";
            printToScreen(node->left, indent + 4);
        }
    }
}

//7. Освобождение памяти (удаление всего дерева)
void freeTree(TreeNode* node) {
    if (node) {
        freeTree(node->left);
        freeTree(node->right);
        node->lineNumbers.dispose(node->head);
        delete node;
    }
}

//функция для загрузки данных из файла в дерево
bool loadDataFromFile(const string& filename, AVLTree& tree) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Ошибка в открытии файла: " << filename << endl;
        return false;
    }

    string line;
    int lineNumber = 1;
    while (getline(inputFile, line)) {
        string lastName1, firstName1, middleName1, lastName2, firstName2, middleName2;

        stringstream ss(line);

        if (ss >> lastName1 >> firstName1 >> middleName1 >> lastName2 >> firstName2 >> middleName2) {
            string fullName1 = lastName1 + " " + firstName1 + " " + middleName1;
            string fullName2 = lastName2 + " " + firstName2 + " " + middleName2;

            if (isValidFullName(fullName1) && isValidFullName(fullName2)) {
                PersonName person1 = { lastName1, firstName1, middleName1 };
                PersonName person2 = { lastName2, firstName2, middleName2 };
                Key key = { person1, person2 };
                bool h = false;
                tree.root = insert(tree.root, key, lineNumber, h);
            }
            else {
                cerr << "Ошибка: некорректный формат ФИО в строке " << lineNumber << endl;
            }
        }
        else {
            cerr << "Ошибка формата данных в строке: " << lineNumber << endl;
        }

        lineNumber++;
    }

    inputFile.close();
    return true;
}

//1. Инициализация (пустого дерева)
void initializeTree(AVLTree& tree) {
    freeTree(tree.root);
    tree.root = nullptr;
    cout << "Дерево инициализировано (пустое).\n";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");

    AVLTree tree;
    int choice;

    //загрузка данных из файла при запуске
    if (!loadDataFromFile("input.txt", tree)) {
        return 1;
    }

    do {
        cout << "\nМеню:\n";
        cout << "1. Инициализация (пустого дерева)\n";
        cout << "2. Добавить новый элемент\n";
        cout << "3. Удалить элемент (максимальный слева) (при полном совпадении ключа и номера строки)\n";
        cout << "4. Найти элемент\n";
        cout << "5. Распечатать дерево в файл (в порядке прямого обхода)\n";
        cout << "6. Освободить память (удалить дерево)\n";
        cout << "7. Распечатать дерево на экран\n";
        cout << "0. Выход\n";
        cout << "Введите ваш выбор: ";
        cin >> choice;

        switch (choice) {
        case 1:
            initializeTree(tree);
            break;

        case 2: {
            Key key;
            int lineNumber;

            do {
                cout << "Введите данные первого человека (Фамилия Имя Отчество): ";
                cin >> key.person1.lastName >> key.person1.firstName >> key.person1.middleName;
                string fullName1 = key.person1.lastName + " " + key.person1.firstName + " " + key.person1.middleName;
                if (!isValidFullName(fullName1)) {
                    cout << "Ошибка: некорректное ФИО. Повторите ввод.\n";
                }
                else {
                    break;
                }
            } while (true);

            do {
                cout << "Введите данные второго человека (Фамилия Имя Отчество): ";
                cin >> key.person2.lastName >> key.person2.firstName >> key.person2.middleName;
                string fullName2 = key.person2.lastName + " " + key.person2.firstName + " " + key.person2.middleName;
                if (!isValidFullName(fullName2)) {
                    cout << "Ошибка: некорректное ФИО. Повторите ввод.\n";
                }
                else {
                    break;
                }
            } while (true);

            cout << "Введите номер строки: ";
            cin >> lineNumber;

            bool h = false;
            tree.root = insert(tree.root, key, lineNumber, h);
            cout << "Элемент добавлен.\n";
            break;
        }

        case 3: {
            Key key;

            do {
                cout << "Введите данные первого человека (Фамилия Имя Отчество): ";
                cin >> key.person1.lastName >> key.person1.firstName >> key.person1.middleName;
                string fullName1 = key.person1.lastName + " " + key.person1.firstName + " " + key.person1.middleName;
                if (!isValidFullName(fullName1)) {
                    cout << "Ошибка: некорректное ФИО. Повторите ввод.\n";
                }
                else {
                    break;
                }
            } while (true);

            do {
                cout << "Введите данные второго человека (Фамилия Имя Отчество): ";
                cin >> key.person2.lastName >> key.person2.firstName >> key.person2.middleName;
                string fullName2 = key.person2.lastName + " " + key.person2.firstName + " " + key.person2.middleName;
                if (!isValidFullName(fullName2)) {
                    cout << "Ошибка: некорректное ФИО. Повторите ввод.\n";
                }
                else {
                    break;
                }
            } while (true);

            int lineNumber;
            cout << "Введите номер строки для удаления: ";
            cin >> lineNumber;

            bool h = false;
            tree.root = deleteNode(tree.root, key, lineNumber, h);
            cout << "Элемент удален (если найден ключ и номер строки).\n";
            break;
        }

        case 4: {
            Key key;

            do {
                cout << "Введите данные первого человека (Фамилия Имя Отчество): ";
                cin >> key.person1.lastName >> key.person1.firstName >> key.person1.middleName;
                string fullName1 = key.person1.lastName + " " + key.person1.firstName + " " + key.person1.middleName;
                if (!isValidFullName(fullName1)) {
                    cout << "Ошибка: некорректное ФИО. Повторите ввод.\n";
                }
                else {
                    break;
                }
            } while (true);

            do {
                cout << "Введите данные второго человека (Фамилия Имя Отчество): ";
                cin >> key.person2.lastName >> key.person2.firstName >> key.person2.middleName;
                string fullName2 = key.person2.lastName + " " + key.person2.firstName + " " + key.person2.middleName;
                if (!isValidFullName(fullName2)) {
                    cout << "Ошибка: некорректное ФИО. Повторите ввод.\n";
                }
                else {
                    break;
                }
            } while (true);

            TreeNode* foundNode = search(tree.root, key);
            if (foundNode) {
                cout << "Элемент найден. Номера строк: ";
                foundNode->lineNumbers.print(foundNode->head);
                cout << endl;
            }
            else {
                cout << "Элемент не найден.\n";
            }
            break;
        }

        case 5:
            printToFile(tree, "output.txt");
            cout << "Дерево распечатано в файл output.txt\n";
            break;

        case 6:
            if (tree.root == nullptr) cout << "Дерево пустое.\n";
            else {
                freeTree(tree.root);
                tree.root = nullptr;;
                cout << "Память дерева освобождена.\n";
            }
            break;

        case 7:
            cout << "Дерево:\n";
            if (tree.root == nullptr) cout << "Дерево пустое\n";
            printToScreen(tree.root);
            break;

        case 0:
            cout << "Выход.\n";
            break;

        default:
            cout << "Неверный выбор.\n";
        }
    } while (choice != 0);

    freeTree(tree.root);
    tree.root = nullptr;
    return 0;
}
