#include <iostream> // To interact with users
#include <string> // Strings are sometimes

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
				cout << "Warning: int (" << key << ") already exists. Record will not be added." << endl;

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
			cout << "Warning: Could not find a slot to add key (" << key << "). Table expansion required.\n";
			// Can be removed if you really don't want to insert keys
			resize(true);
			add(key, index);
		}

		return false;
	}

	bool remove(const int& key, string subject, string date) {
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

		cout << "Table size changed from " << oldCapacity << " to " << capacity << endl;
		cout << "Rehashing...\n";

		for (int i = 0; i < oldCapacity; i++) {
			if (oldTable[i].status == 1) {
				add(oldTable[i].key, oldTable[i].index);
			}
		}
		delete[] oldTable;
	}

	const Item* search(const int& key) const {
		int index = primaryHash(key);
		int attempt = 0;

		while (attempt < capacity) {
			int probedIndex = secondaryHash(index, attempt);

			if (table[probedIndex].status == 0) return nullptr;
			else if (table[probedIndex].status == 1 && table[probedIndex].key == key) return &table[probedIndex];

			attempt += 1;
		}

		return nullptr;
	}

	~HashTable() {
		delete[] table;
	}
};