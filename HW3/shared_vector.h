//
// Created by daniil on 31.05.19.
//

#ifndef BIGINT_SHARED_VECTOR_H
#define BIGINT_SHARED_VECTOR_H

#include <cstddef>
#include <cstring>
#include "shared_ptr.h"
#include <vector>
// #include <iostream>

template<typename T>
class SharedVector {
    static const size_t smallsize = 2;

    size_t size_, capacity;
    T small_object[smallsize];
    SharedPtr<T> big_object;

    void makeBig(size_t new_capacity = smallsize * 3) {
        capacity = new_capacity;
        big_object.reset((T *) (operator new[](capacity * sizeof(T))));
        for (size_t i = 0; i < size_; ++i) {
            new(big_object.get() + i) T(small_object[i]);
        }
        for (size_t i = 0; i < size_; ++i) {
            small_object[i].~T();
        }
    }

    void make_copy() {
        T *big_object_copy = (T*) operator new[](capacity * sizeof(T));
        for (size_t i = 0; i < size_; i++) {
            new(big_object_copy + i) T(big_object[i]);
        }
        big_object.reset(big_object_copy);
    }

    void clear_small_object() {
        for (size_t i = 0; i < size_; i++) {
            small_object[i].~T();
        }
    }

public:
    SharedVector() : size_(0), capacity(0) {}

    SharedVector(const SharedVector<T> &x) : size_(x.size_), capacity(x.capacity), big_object(x.big_object) {
        if (big_object.get() == nullptr) {
            for (size_t i = 0; i < size_; ++i) {
                new(small_object + i) T(x.small_object[i]);
            }
        }
    }
    SharedVector(T x) : size_(1), capacity(0) {
        new(small_object) T(x);
    }

    SharedVector(size_t sz, T x) : size_(0), capacity(0) {
        for (size_t i = 0; i < sz; ++i) {
            this->push_back(x);
        }
    }

    ~SharedVector() {
        if (big_object.get()) { //capacity == 0?
            clear_small_object();
        } else {
            for (size_t i = 0; i < capacity; i++) {
                big_object[i].~T();
            }
        }
    }

    SharedVector &operator=(const SharedVector<T> &x) {
        if (x.big_object.get()) {
            if (capacity == 0) {
                clear_small_object();
            }
            big_object = x.big_object;
        } else {
            for (size_t i = 0; i < x.size_; i++) {
                new(small_object + i) T(x.small_object[i]);
            }
            big_object.reset();
        }
        size_ = x.size_;
        capacity = x.capacity;
        return *this;
    }

    SharedVector (const std::vector<T> &x) {
        size_t xsz = x.size();
        if (xsz > smallsize) {
            T *big_object_copy = (T*) operator new[](x.size() * sizeof(T));
            for (size_t i = 0; i < xsz; i++) {
                new(big_object_copy + i) T(x[i]);
            }
            big_object.reset(big_object_copy);
        } else {
            for (size_t i = 0; i < xsz; i++) {
                new(small_object + i) T(x[i]);
            }
            big_object.reset();
        }
        size_ = x.size();
        capacity = (size_ <= 2) ? 0 : size_;
    }

    size_t size() const {
        return size_;
    }

    T back() const {
        return (*this)[size_ - 1];
    }

    bool empty() const {
        return size_ == 0;
    }

    void push_back(T val) {
        if (size_ == smallsize && big_object.get() == nullptr) {
            makeBig();
        }
        if (big_object.get() == nullptr) {
            new(small_object + size_) T(val);
        } else {
            if (capacity == size_) {
                capacity *= 2;
                make_copy();
            } else if (big_object.not_unique()) {
                make_copy();
            }
            T* ptr = big_object.get();
            new(ptr + size_) T(val);
        }
        ++size_;
    }

    void pop_back() {
        if (big_object.not_unique()) {
            make_copy();
        }
        --size_;
    }

    T &operator[](size_t i) {
        if (capacity > 0) {
            if (big_object.not_unique()) {
                make_copy();
            }
            return big_object[i];
        } else {
            return small_object[i];
        }
    }

    const T &operator[](size_t i) const {
        return (capacity > 0) ? big_object[i] : small_object[i];
    }

};

#endif //BIGINT_SHARED_VECTOR_H
