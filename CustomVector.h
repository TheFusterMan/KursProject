#pragma once

#include <stdexcept>    // Для std::out_of_range
#include <algorithm>    // Для std::copy
#include <iterator>     // Для std::reverse_iterator

// --- НАЧАЛО: САМОПИСНЫЙ КЛАСС ВЕКТОРА ---

template <typename T>
class CustomVector {
private:
    T* data;
    size_t currentSize;
    size_t currentCapacity;

    // Вспомогательная функция для увеличения размера внутреннего массива
    void reallocate(size_t newCapacity) {
        if (newCapacity <= currentSize) {
            return;
        }

        T* newData = new T[newCapacity];
        if (data) {
            // Копируем старые элементы в новый массив
            std::copy(data, data + currentSize, newData);
            delete[] data;
        }

        data = newData;
        currentCapacity = newCapacity;
    }

public:
    // Конструктор по умолчанию
    CustomVector() : data(nullptr), currentSize(0), currentCapacity(0) {}

    // Деструктор для освобождения памяти
    ~CustomVector() {
        delete[] data;
    }

    // Конструктор копирования (Правило трёх)
    CustomVector(const CustomVector& other)
        : data(nullptr), currentSize(0), currentCapacity(0) {
        if (other.currentCapacity > 0) {
            data = new T[other.currentCapacity];
            currentCapacity = other.currentCapacity;
            currentSize = other.currentSize;
            std::copy(other.data, other.data + other.currentSize, data);
        }
    }

    // Оператор присваивания копированием (Правило трёх)
    CustomVector& operator=(const CustomVector& other) {
        if (this == &other) {
            return *this; // Защита от самоприсваивания
        }

        delete[] data;
        data = nullptr;
        currentSize = 0;
        currentCapacity = 0;

        if (other.currentCapacity > 0) {
            data = new T[other.currentCapacity];
            currentCapacity = other.currentCapacity;
            currentSize = other.currentSize;
            std::copy(other.data, other.data + other.currentSize, data);
        }
        return *this;
    }

    // Добавление элемента в конец
    void append(const T& value) {
        if (currentSize >= currentCapacity) {
            // Если места нет, увеличиваем ёмкость (например, в 2 раза)
            reallocate(currentCapacity == 0 ? 10 : currentCapacity * 2);
        }
        data[currentSize++] = value;
    }

    // Доступ по индексу (без проверки границ)
    T& operator[](size_t index) {
        return data[index];
    }

    // Доступ по индексу (для константных объектов)
    const T& operator[](size_t index) const {
        return data[index];
    }

    // Доступ по индексу (с проверкой границ)
    const T& at(size_t index) const {
        if (index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    // Получение текущего количества элементов
    size_t size() const {
        return currentSize;
    }

    // Очистка вектора (не освобождает память)
    void clear() {
        currentSize = 0;
    }

    // Возвращает последний элемент
    T& last() {
        if (currentSize == 0) {
            throw std::out_of_range("Vector is empty");
        }
        return data[currentSize - 1];
    }

    const T& last() const {
        if (currentSize == 0) {
            throw std::out_of_range("Vector is empty");
        }
        return data[currentSize - 1];
    }

    // Удаляет последний элемент
    void removeLast() {
        if (currentSize > 0) {
            currentSize--;
        }
    }

    // Проверка, пуст ли вектор
    bool isEmpty() const {
        return currentSize == 0;
    }

    // --- Итераторы для совместимости с std::sort ---
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { return data; }
    iterator end() { return data + currentSize; }

    const_iterator begin() const { return data; }
    const_iterator end() const { return data + currentSize; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
};
