#include <iostream>
#include <string>
#include <clocale>

using namespace std;

struct ListNode {
    int data;
    ListNode* next;
};

class LinkedList {
public:
    ListNode* init() {
        return nullptr;
    }

    //2. Освобождение памяти
    void dispose(ListNode*& head) {
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

    //3. Добавление элемента (упорядоченный по возрастанию)
    void addSorted(ListNode*& head, int value) {
        ListNode* newNode = new ListNode{ value, nullptr };

        if (!head || value < head->data) {
            newNode->next = head;
            head = newNode;
            return;
        }

        ListNode* current = head;
        while (current->next && current->next->data < value) {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    //4. Удаление всех вхождений заданного по значению элемента
    void removeAll(ListNode*& head, int value) {
        while (head && head->data == value) {
            ListNode* temp = head;
            head = temp->next;
            delete temp;
            temp = nullptr;
        }

        ListNode* current = head;
        while (current && current->next) {
            if (current->next->data == value) {
                ListNode* temp = current->next;
                current->next = temp->next;
                delete temp;
                temp = nullptr;
            }
            else {
                current = current->next;
            }
        }
    }

    //6. Поиск заданного элемента по значению
    bool find(ListNode* head, int value) {
        ListNode* current = head;
        while (current) {
            if (current->data == value)
                return true;
            current = current->next;
        }
        return false;
    }

    // для печати списка
    string toString(ListNode* head) const {
        string result = "";
        ListNode* current = head;
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