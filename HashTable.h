﻿#include <string> //для вывода
#include <sstream> //для вывода

using namespace std;

struct Item {
	unsigned long long key; 
	int index;
	int status; //0 - свободна, 1 - занята, 2 - удалена

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
	}

	unsigned long long primaryHash(const unsigned long long& key) const {
		unsigned long long num = 0;
		unsigned long long temp_key = key;

		while (temp_key > 0) {
			num += temp_key % 10;
			temp_key /= 10;
		}

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

	bool add(const unsigned long long& key, int index) {
		int initIndex = primaryHash(key);
		int potentialIndex = -1;
		int attempt = 0;

		while (attempt < capacity) {
			int probedIndex = secondaryHash(initIndex, attempt);

			if (table[probedIndex].status == 0 && potentialIndex == -1) {
				table[probedIndex].key = key;
				table[probedIndex].index = index;
				table[probedIndex].status = 1;
				size += 1;

				if (static_cast<double>(size) / capacity >= MAX_FILL_FACTOR) {
					resize(true);
				}

				return true;
			}
			else if (table[probedIndex].status == 2) {
				if (potentialIndex == -1) potentialIndex = probedIndex;
			}
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) {
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
			resize(true);
			add(key, index);
		}

		return false;
	}

	bool remove(const unsigned long long& key) {
		int index = primaryHash(key);
		int attempt = 0;

		while (attempt < capacity) {
			int probedIndex = secondaryHash(index, attempt);

			if (table[probedIndex].status == 0) return false;
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) {
				table[probedIndex].status = 2;
				size -= 1;

				if (capacity > INIT_CAPACITY && static_cast<double>(size) / capacity <= MIN_FILL_FACTOR) {
					resize(false);
				}

				return true;
			}

			attempt += 1;
		}

		return false;
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

		capacity = newActualCapacity;
		size = 0;
		table = new Item[capacity];

		for (int i = 0; i < oldCapacity; i++) {
			if (oldTable[i].status == 1) {
				add(oldTable[i].key, oldTable[i].index);
			}
		}
		delete[] oldTable;
	}

	const Item* search(const unsigned long long& key, int& steps) const {
		int index = primaryHash(key);
		int attempt = 0;
		steps = 0;

		while (attempt < capacity) {
			steps += 1;
			int probedIndex = secondaryHash(index, attempt);

			if (table[probedIndex].status == 0) return nullptr;
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) return &table[probedIndex];

			attempt += 1;
		}

		return nullptr;
	}

	string toString() const {
		stringstream ss;

		ss << "--- Хэш-таблица содержит " << size << " из " << capacity << " записей (" + std::to_string(static_cast<int>(size / capacity * 100)) + "% занято) ---\n";

		for (int i = 0; i < capacity; ++i) {
			ss << "[" << i << "]: ";

			unsigned long long hash = 0;
			unsigned long long temp_key = table[i].key;

			while (temp_key > 0) {
				hash += temp_key % 10;
				temp_key /= 10;
			}

			string squaredHash = to_string(hash * hash);

			ss << "Статус: ";
			switch (table[i].status) {
			case 0:
				ss << "0\n";
				break;
			case 1:
				ss << "1 | ИНН (ключ): " << table[i].key  << " - " << primaryHash(table[i].key) << " - " << hash << " - " << squaredHash << " | Индекс: " << table[i].index;
				ss << "\n";
				break;
			case 2:
				ss << "2 | ИНН (ключ) : " << table[i].key  << " - " << primaryHash(table[i].key) << " - " << hash << " - " << squaredHash << " | Индекс : " << table[i].index;
				ss << "\n";
				break;
			}
		}

		ss << "---------------------------------------------------------------------------\n";
		return ss.str();
	}

	~HashTable() {
		delete[] table;
	}
};