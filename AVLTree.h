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
    //1. �������������
    Node* init() {
        return nullptr;
    }

    //2. ������������ ������
    void dispose(Node*& head) {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

    //3. ���������� �������� (������������� �� �����������)
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

    //4. �������� ���� ��������� ��������� �� �������� ��������
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

    //5. �������� (����� ������� ��������� ���������)
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

    //6. ����� ��������� �������� �� ��������
    bool find(Node* head, int value) {
        Node* current = head;
        while (current) {
            if (current->data == value)
                return true;
            current = current->next;
        }
        return false;
    }

    //7. ������
    void print(Node* head) {
        Node* current = head;
        while (current) {
            cout << current->data;
            if (current->next) cout << ", ";
            current = current->next;
        }
        cout << endl;
    }

    //8. �������� ������ � ����� �������� (�������������� ��������)
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

    //9 (10). ������������� �������� ������
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

    //��� ������ ������
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

//��������� ��� ������������� ����� (��� + ���)
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

//�������� ������������ ���
bool isValidFullName(const string& fullName) {
    regex fullNameRegex(R"(^[�-ߨ][�-��]+ [�-ߨ][�-��]+ [�-ߨ][�-��]+$)");

    return regex_match(fullName, fullNameRegex);
}

//3. �������� ��������� �������� (������������ �����)
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
        if (p1->balanceFactor >= 0) { //��������� RR
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
        else { //������� RL
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
        if (p1->balanceFactor <= 0) { //��������� LL
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
        else { //������� LR
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
        //�������� ������ ������� �����
        q->lineNumbers.dispose(q->head); //������� ������ ������
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

//3. �������� ��������� �������� (������������ �����) (��� ������ ���������� ����� � ������ ������)
TreeNode* deleteNode(TreeNode* p, const Key& key, int lineNumber, bool& h) {
    TreeNode* q;

    if (p == nullptr) {
        cout << "������� �� ������" << endl;
    }
    else if (p->key > key) {
        p->left = deleteNode(p->left, key, lineNumber, h);
        if (h) p = balanceL(p, h);
    }
    else if (p->key < key) {
        p->right = deleteNode(p->right, key, lineNumber, h);
        if (h) p = balanceR(p, h);
    }
    else { //���� ������
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
                    q->left = del(q->left, q, h); //����� max � ����� ���������
                    if (h) p = balanceL(p, h);
                }
            }
            else {
                h = false;
            }
        }
        else {
            cout << "����� ������ �� ������ ��� ������� �����" << endl;
            h = false;
        }
    }
    return p;
}

//2. ���������� ������ ��������
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

//4. ����� ��������� ��������
TreeNode* search(TreeNode* node, const Key& key) {
    if (node == nullptr || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return search(node->left, key);
    }

    return search(node->right, key);
}

//5. ������
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
        cerr << "������ � �������� �����: " << filename << endl;
    }
}

//6. �����: ������
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

//7. ������������ ������ (�������� ����� ������)
void freeTree(TreeNode* node) {
    if (node) {
        freeTree(node->left);
        freeTree(node->right);
        node->lineNumbers.dispose(node->head);
        delete node;
    }
}

//������� ��� �������� ������ �� ����� � ������
bool loadDataFromFile(const string& filename, AVLTree& tree) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "������ � �������� �����: " << filename << endl;
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
                cerr << "������: ������������ ������ ��� � ������ " << lineNumber << endl;
            }
        }
        else {
            cerr << "������ ������� ������ � ������: " << lineNumber << endl;
        }

        lineNumber++;
    }

    inputFile.close();
    return true;
}

//1. ������������� (������� ������)
void initializeTree(AVLTree& tree) {
    freeTree(tree.root);
    tree.root = nullptr;
    cout << "������ ���������������� (������).\n";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");

    AVLTree tree;
    int choice;

    //�������� ������ �� ����� ��� �������
    if (!loadDataFromFile("input.txt", tree)) {
        return 1;
    }

    do {
        cout << "\n����:\n";
        cout << "1. ������������� (������� ������)\n";
        cout << "2. �������� ����� �������\n";
        cout << "3. ������� ������� (������������ �����) (��� ������ ���������� ����� � ������ ������)\n";
        cout << "4. ����� �������\n";
        cout << "5. ����������� ������ � ���� (� ������� ������� ������)\n";
        cout << "6. ���������� ������ (������� ������)\n";
        cout << "7. ����������� ������ �� �����\n";
        cout << "0. �����\n";
        cout << "������� ��� �����: ";
        cin >> choice;

        switch (choice) {
        case 1:
            initializeTree(tree);
            break;

        case 2: {
            Key key;
            int lineNumber;

            do {
                cout << "������� ������ ������� �������� (������� ��� ��������): ";
                cin >> key.person1.lastName >> key.person1.firstName >> key.person1.middleName;
                string fullName1 = key.person1.lastName + " " + key.person1.firstName + " " + key.person1.middleName;
                if (!isValidFullName(fullName1)) {
                    cout << "������: ������������ ���. ��������� ����.\n";
                }
                else {
                    break;
                }
            } while (true);

            do {
                cout << "������� ������ ������� �������� (������� ��� ��������): ";
                cin >> key.person2.lastName >> key.person2.firstName >> key.person2.middleName;
                string fullName2 = key.person2.lastName + " " + key.person2.firstName + " " + key.person2.middleName;
                if (!isValidFullName(fullName2)) {
                    cout << "������: ������������ ���. ��������� ����.\n";
                }
                else {
                    break;
                }
            } while (true);

            cout << "������� ����� ������: ";
            cin >> lineNumber;

            bool h = false;
            tree.root = insert(tree.root, key, lineNumber, h);
            cout << "������� ��������.\n";
            break;
        }

        case 3: {
            Key key;

            do {
                cout << "������� ������ ������� �������� (������� ��� ��������): ";
                cin >> key.person1.lastName >> key.person1.firstName >> key.person1.middleName;
                string fullName1 = key.person1.lastName + " " + key.person1.firstName + " " + key.person1.middleName;
                if (!isValidFullName(fullName1)) {
                    cout << "������: ������������ ���. ��������� ����.\n";
                }
                else {
                    break;
                }
            } while (true);

            do {
                cout << "������� ������ ������� �������� (������� ��� ��������): ";
                cin >> key.person2.lastName >> key.person2.firstName >> key.person2.middleName;
                string fullName2 = key.person2.lastName + " " + key.person2.firstName + " " + key.person2.middleName;
                if (!isValidFullName(fullName2)) {
                    cout << "������: ������������ ���. ��������� ����.\n";
                }
                else {
                    break;
                }
            } while (true);

            int lineNumber;
            cout << "������� ����� ������ ��� ��������: ";
            cin >> lineNumber;

            bool h = false;
            tree.root = deleteNode(tree.root, key, lineNumber, h);
            cout << "������� ������ (���� ������ ���� � ����� ������).\n";
            break;
        }

        case 4: {
            Key key;

            do {
                cout << "������� ������ ������� �������� (������� ��� ��������): ";
                cin >> key.person1.lastName >> key.person1.firstName >> key.person1.middleName;
                string fullName1 = key.person1.lastName + " " + key.person1.firstName + " " + key.person1.middleName;
                if (!isValidFullName(fullName1)) {
                    cout << "������: ������������ ���. ��������� ����.\n";
                }
                else {
                    break;
                }
            } while (true);

            do {
                cout << "������� ������ ������� �������� (������� ��� ��������): ";
                cin >> key.person2.lastName >> key.person2.firstName >> key.person2.middleName;
                string fullName2 = key.person2.lastName + " " + key.person2.firstName + " " + key.person2.middleName;
                if (!isValidFullName(fullName2)) {
                    cout << "������: ������������ ���. ��������� ����.\n";
                }
                else {
                    break;
                }
            } while (true);

            TreeNode* foundNode = search(tree.root, key);
            if (foundNode) {
                cout << "������� ������. ������ �����: ";
                foundNode->lineNumbers.print(foundNode->head);
                cout << endl;
            }
            else {
                cout << "������� �� ������.\n";
            }
            break;
        }

        case 5:
            printToFile(tree, "output.txt");
            cout << "������ ����������� � ���� output.txt\n";
            break;

        case 6:
            if (tree.root == nullptr) cout << "������ ������.\n";
            else {
                freeTree(tree.root);
                tree.root = nullptr;;
                cout << "������ ������ �����������.\n";
            }
            break;

        case 7:
            cout << "������:\n";
            if (tree.root == nullptr) cout << "������ ������\n";
            printToScreen(tree.root);
            break;

        case 0:
            cout << "�����.\n";
            break;

        default:
            cout << "�������� �����.\n";
        }
    } while (choice != 0);

    freeTree(tree.root);
    tree.root = nullptr;
    return 0;
}
