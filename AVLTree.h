#include <iostream>
#include <string>

#include <LinkedList.h>

using namespace std;

template <typename TKey>
struct TreeNode {
    TKey key;
    IndexList indexList;
    ListNode* head;
    int balanceFactor;
    TreeNode<TKey>* left;
    TreeNode<TKey>* right;

    TreeNode(const TKey& k, int idx) : key(k), balanceFactor(0), left(nullptr), right(nullptr), head(nullptr) {
        indexList.addSorted(head, idx);
    }

    ~TreeNode() {
        indexList.dispose(head);
    }
};

template <typename TKey>
class AVLTree {
public:
    TreeNode<TKey>* root;

    AVLTree() : root(nullptr) {}
    ~AVLTree() {
        freeTree(root);
    }

    //3. Удаление заданного элемента (максимальный слева)
    TreeNode<TKey>* balanceL(TreeNode<TKey>* p, bool& h) {
        TreeNode<TKey>* p1;
        TreeNode<TKey>* p2;
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

    TreeNode<TKey>* balanceR(TreeNode<TKey>* p, bool& h) {
        TreeNode<TKey>* p1;
        TreeNode<TKey>* p2;

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

    TreeNode<TKey>* del(TreeNode<TKey>* r, TreeNode<TKey>* q, bool& h) {
        if (r->right != nullptr) {
            r->right = del(r->right, q, h);
            if (h) r = balanceR(r, h);
        }
        else {
            q->key = r->key;
            //копируем список номеров строк
            q->indexList.dispose(q->head); //очищаем старый список
            ListNode* current = r->head;
            while (current) {
                q->indexList.addSorted(q->head, current->data);
                current = current->next;
            }

            q = r;
            r = r->left;
            h = true;
        }
        return r;
    }

    //3. Удаление заданного элемента (максимальный слева) (при полном совпадении ключа и номера строки)
    TreeNode<TKey>* deleteNode(TreeNode<TKey>* p, const TKey& key, int indexList, bool& h) {
        TreeNode<TKey>* q;

        if (p == nullptr) {
            //cout << "Элемент не найден" << endl;
        }
        else if (p->key > key) {
            p->left = deleteNode(p->left, key, indexList, h);
            if (h) p = balanceL(p, h);
        }
        else if (p->key < key) {
            p->right = deleteNode(p->right, key, indexList, h);
            if (h) p = balanceR(p, h);
        }
        else { //ключ найден
            if (p->indexList.find(p->head, indexList)) {
                p->indexList.removeAll(p->head, indexList);

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

    void remove(const TKey& key, int index) {
        bool h = false;
        root = deleteNode(root, key, index, h);
    }

    //2. Добавление нового элемента
    TreeNode<TKey>* insert(TreeNode<TKey>* p, const TKey& key, int index, bool& h) {
        TreeNode<TKey>* p1;
        TreeNode<TKey>* p2;

        if (p == nullptr) {
            p = new TreeNode<TKey>(key, index);
            h = true;
        }
        else if (p->key > key) {
            p->left = insert(p->left, key, index, h);
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
            p->right = insert(p->right, key, index, h);
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
            p->indexList.addSorted(p->head, index);
            h = false;
        }
        return p;
    }

    void add(const TKey& key, int index) {
        bool h = false;
        root = insert(root, key, index, h);
    }

    //4. Поиск заданного элемента
    TreeNode<TKey>* search(TreeNode<TKey>* node, const TKey& key) {
        if (node == nullptr || node->key == key) {
            return node;
        }

        if (key < node->key) {
            return search(node->left, key);
        }

        return search(node->right, key);
    }

    TreeNode<TKey>* find(const TKey& key) {
        return search(root, key);
    }

    //7. Освобождение памяти (удаление всего дерева)
    void freeTree(TreeNode<TKey>* node) {
        if (node) {
            freeTree(node->left);
            freeTree(node->right);
            node->indexList.dispose(node->head);
            delete node;
        }
    }

    //1. Инициализация (пустого дерева)
    void initializeTree(AVLTree<TKey>& tree) {
        freeTree(tree.root);
        tree.root = nullptr;
        cout << "Дерево инициализировано (пустое).\n";
    }
};