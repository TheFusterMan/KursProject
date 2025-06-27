#include <iostream> // To interact with users
#include <string> // Strings are sometimes
#include "DebugLogger.h"

using namespace std;

struct Item {
	ulong key; 
	int index;
	int status; //0 - free, 1 - occupied, 2 - deleted

	Item() : key(), index(-1), status(0) {}
	Item(const int& _key, int _index) : key(_key), index(_index) {}
};

class HashTable {
private:
	const double MAX_FILL_FACTOR = 0.75;
	const double MIN_FILL_FACTOR = 0.25;
	const int INIT_CAPACITY;
	const int k1 = 3, k2 = 4;

	Item* table;
	int capacity;
	double size;


public:
	HashTable(int initCapHint) : INIT_CAPACITY(initCapHint), size(0) {
		capacity = INIT_CAPACITY;
		table = new Item[capacity];
		DebugLogger::log(QString("HashTable: Инициализирована с capacity = %1").arg(capacity));
	}

	~HashTable() {
		delete[] table;
		DebugLogger::log("HashTable: Уничтожена.");
	}

	unsigned long long keyToNum(const int& key) const {
		unsigned long long num = 0;
		for (char c : to_string(key)) num += c;
		return num;
	}

	unsigned long long primaryHash(const int& key) const {
		unsigned long long num = keyToNum(key);
		string sqNumStr = to_string(num * num);

		int len = to_string(capacity - 1).length();
		int pos = (sqNumStr.length() / 2) - (len / 2);

		if (pos < 0) pos = 0;
		if (pos + len > sqNumStr.length()) {
			len = sqNumStr.length() - pos;
			if (len < 0) len = 0;
		}

		unsigned long long addr = stoull(sqNumStr.substr(pos, len));

		return addr % capacity;
	}

	int secondaryHash(int initHash, int attempt) const {
		return (initHash + attempt * k1 + attempt * attempt * k2) % capacity;
	}

	bool add(const int& key, int index) {
		int initIndex = primaryHash(key);
		int potentialIndex = -1;
		int attempt = 0;

		DebugLogger::log(QString("HashTable ADD: Попытка добавить ключ %1 (индекс %2). Начальный хеш: %3").arg(key).arg(index).arg(initIndex));

		while (attempt < capacity) {
			int probedIndex = secondaryHash(initIndex, attempt);

			if (table[probedIndex].status == 0 && potentialIndex == -1) {
				table[probedIndex].key = key;
				table[probedIndex].index = index;
				table[probedIndex].status = 1;
				size += 1;

				DebugLogger::log(QString("HashTable ADD: Ключ %1 добавлен в ячейку %2").arg(key).arg(probedIndex));

				if (static_cast<double>(size) / capacity >= MAX_FILL_FACTOR) {
					resize(true);
				}

				return true;
			}
			else if (table[probedIndex].status == 2) {
				if (potentialIndex == -1) potentialIndex = probedIndex;
			}
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) {
				DebugLogger::log(QString("HashTable WARNING: Ключ %1 уже существует. Добавление отменено.").arg(key));

				return false;
			}

			attempt += 1;
		}

		if (potentialIndex != -1) {
			table[potentialIndex].key = key;
			table[potentialIndex].index = index;
			table[potentialIndex].status = 1;
			size += 1;

			if (size / capacity >= MAX_FILL_FACTOR) {
				resize(true);
			}

			return true;
		}
		else {
			DebugLogger::log(QString("HashTable ERROR: Не удалось найти место для ключа %1. Таблица заполнена?").arg(key));

			resize(true);
			add(key, index);
		}

		return false;
	}

	bool remove(const int& key) {
		int index = primaryHash(key);
		int attempt = 0;
		DebugLogger::log(QString("HashTable REMOVE: Попытка удалить ключ %1.").arg(key));

		while (attempt < capacity) {
			int probedIndex = secondaryHash(index, attempt);

			if (table[probedIndex].status == 0) {
				DebugLogger::log(QString("HashTable REMOVE: Ключ %1 не найден (пустая ячейка).").arg(key));
				return false;
			}
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) {
				table[probedIndex].status = 2;
				size -= 1;
				DebugLogger::log(QString("HashTable REMOVE: Ключ %1 удален (помечен как 'deleted').").arg(key));

				if (capacity > INIT_CAPACITY && static_cast<double>(size) / capacity <= MIN_FILL_FACTOR) {
					resize(false);
				}

				return true;
			}

			attempt += 1;
		}
		DebugLogger::log(QString("HashTable REMOVE: Ключ %1 не найден после полного сканирования.").arg(key));
		return false;
	}

	bool updateIndex(const int& key, int new_index) {
		int initIndex = primaryHash(key);
		int attempt = 0;
		DebugLogger::log(QString("HashTable UPDATE: Поиск ключа %1 для обновления индекса на %2.").arg(key).arg(new_index));

		while (attempt < capacity) {
			int probedIndex = secondaryHash(initIndex, attempt);

			// Если наткнулись на пустую ячейку, значит ключа в таблице нет
			if (table[probedIndex].status == 0) {
				DebugLogger::log(QString("HashTable UPDATE: Ключ %1 не найден.").arg(key));
				return false;
			}

			// Если нашли ячейку с нужным ключом
			if (table[probedIndex].status == 1 && table[probedIndex].key == key) {
				table[probedIndex].index = new_index; // Обновляем индекс
				DebugLogger::log(QString("HashTable UPDATE: Индекс для ключа %1 успешно обновлен на %2.").arg(key).arg(new_index));
				return true; // Успех
			}

			attempt++;
		}

		return false; // Ключ не найден после полного прохода
	}

	void resize(bool isExpands) {
		int oldCapacity = capacity;
		Item* oldTable = table;

		int newCapacityTarget;
		if (isExpands) {
			newCapacityTarget = oldCapacity * 2;
		}
		else {
			newCapacityTarget = oldCapacity / 2;
		}

		int newActualCapacity = newCapacityTarget;

		if (newActualCapacity == oldCapacity) {
			return;
		}

		DebugLogger::log(QString("HashTable RESIZE: Изменение размера с %1 на %2.").arg(oldCapacity).arg(capacity));

		capacity = newActualCapacity;
		size = 0;
		table = new Item[capacity];

		DebugLogger::log("HashTable RESIZE: Начато перехеширование...");
		for (int i = 0; i < oldCapacity; i++) {
			if (oldTable[i].status == 1) {
				add(oldTable[i].key, oldTable[i].index);
			}
		}
		delete[] oldTable;
		DebugLogger::log("HashTable RESIZE: Перехеширование завершено.");
	}

	const Item* search(const int& key) const {
		int index = primaryHash(key);
		int attempt = 0;
		DebugLogger::log(QString("HashTable SEARCH: Поиск ключа %1.").arg(key));

		while (attempt < capacity) {
			int probedIndex = secondaryHash(index, attempt);

			if (table[probedIndex].status == 0) {
				DebugLogger::log(QString("HashTable SEARCH: Ключ %1 не найден (достигнута пустая ячейка).").arg(key));
				return nullptr;
			}
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) {
				DebugLogger::log(QString("HashTable SEARCH: Ключ %1 найден в ячейке %2.").arg(key).arg(probedIndex));
				return &table[probedIndex];
			}

			attempt += 1;
		}
		DebugLogger::log(QString("HashTable SEARCH: Ключ %1 не найден после полного сканирования.").arg(key));
		return nullptr;
	}
};