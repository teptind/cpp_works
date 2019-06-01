//
// Created by daniil on 31.05.19.
//

#ifndef BIGINT_SHARED_PTR_H
#define BIGINT_SHARED_PTR_H

#include <cstdint>

template<typename T>
struct SharedPtr {
    struct Box {
        T *data;
        uint16_t cnt;

        explicit Box(T *data_) {
            data = data_;
            cnt = 1;
        }

        ~Box() {
            operator delete[](data);
        };
    };

    explicit SharedPtr(T *ptr = nullptr) {
        if (ptr) {
            box = new Box(ptr);
        } else {
            box = nullptr;
        }
    }

    SharedPtr(const SharedPtr &a) {
        if (a.box) {
            box = a.box;
            ++box->cnt;
        } else {
            box = nullptr;
        }
    }

    ~SharedPtr() {
        if (box && --box->cnt == 0) {
            delete box;
        }
    }

    SharedPtr &operator=(const SharedPtr &a) {
        if (box && a.box && box == a.box) {
            return *this;
        }
        if (box && --box->cnt == 0) {
            delete box;
        }
        if (a.box) {
            box = a.box;
            ++box->cnt;
        } else {
            box = nullptr;
        }
        return *this;
    }

    void reset(T *ptr = nullptr) {
        if (box && --box->cnt == 0) {
            delete box;
        }
        box = (ptr ? new Box(ptr) : nullptr);
    }

    T *get() const {
        return (box) ? box->data : nullptr;
    }

    T &operator*() {
        return *box->data;
    }

    T *operator->() const {
        return (box) ? box->data : nullptr;
    }

    bool unique() {
        return (box && box->cnt == 1);
    }
    bool not_unique() {
        return (box && box->cnt > 1);
    }

    T &operator[](size_t i) {
        return box->data[i];
    }

    const T &operator[](size_t i) const {
        return box->data[i];
    }

private:
    Box *box;
};

#endif //BIGINT_SHARED_PTR_H
