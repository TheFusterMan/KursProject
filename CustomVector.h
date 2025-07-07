#pragma once

#include <stdexcept>
#include <algorithm>
#include <iterator>

template <typename T>
class CustomVector {
private:
    T* data;
    size_t currentSize;
    size_t currentCapacity;

    void reallocate(size_t newCapacity) {
        if (newCapacity <= currentSize) {
            return;
        }

        T* newData = new T[newCapacity];
        if (data) {
            std::copy(data, data + currentSize, newData);
            delete[] data;
        }

        data = newData;
        currentCapacity = newCapacity;
    }

public:
    CustomVector() : data(nullptr), currentSize(0), currentCapacity(0) {}

    ~CustomVector() {
        delete[] data;
    }

    CustomVector(const CustomVector& other)
        : data(nullptr), currentSize(0), currentCapacity(0) {
        if (other.currentCapacity > 0) {
            data = new T[other.currentCapacity];
            currentCapacity = other.currentCapacity;
            currentSize = other.currentSize;
            std::copy(other.data, other.data + other.currentSize, data);
        }
    }

    CustomVector& operator=(const CustomVector& other) {
        if (this == &other) {
            return *this;
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

    void append(const T& value) {
        if (currentSize >= currentCapacity) {
            reallocate(currentCapacity == 0 ? 10 : currentCapacity * 2);
        }
        data[currentSize++] = value;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    const T& at(size_t index) const {
        if (index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    size_t size() const {
        return currentSize;
    }

    void clear() {
        currentSize = 0;
    }

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

    void removeLast() {
        if (currentSize > 0) {
            currentSize--;
        }
    }

    bool isEmpty() const {
        return currentSize == 0;
    }

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