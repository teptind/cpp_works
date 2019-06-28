//
// Created by daniil on 21.06.19.
//

#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#include <algorithm>
#include <cassert>
#include <cstring>
#include <variant>

template<typename T>
struct VectorIterator {
    typedef std::random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T *pointer;
    typedef T &reference;

    VectorIterator() = default;

    ~VectorIterator() = default;

    VectorIterator(VectorIterator const &) = default;

    VectorIterator &operator=(VectorIterator const &) = default;

    VectorIterator &operator++() {
        ++p;
        return *this;
    }

    const VectorIterator operator++(int) {
        VectorIterator result = *this;
        ++*this;
        return result;
    }

    VectorIterator &operator--() {
        --p;
        return *this;
    }

    const VectorIterator operator--(int) {
        VectorIterator result = *this;
        --*this;
        return result;
    }

    VectorIterator &operator+=(size_t n) {
        p += n;
        return *this;
    }

    VectorIterator &operator-=(size_t n) {
        p -= n;
        return *this;
    }

    reference operator*() const {
        return *p;
    }

    pointer operator->() const {
        return p;
    }

    reference operator[](size_t n) {
        return p[n];
    }

    bool operator==(VectorIterator const &that) const {
        return p == that.p;
    }

    bool operator!=(VectorIterator const &that) const {
        return p != that.p;
    }

    bool operator<(VectorIterator const &that) {
        return p < that.p;
    }

    bool operator>(VectorIterator const &that) {
        return p > that.p;
    }

    bool operator<=(VectorIterator const &that) {
        return p <= that.p;
    }

    bool operator>=(VectorIterator const &that) {
        return p >= that.p;
    }

    friend difference_type operator-(VectorIterator const &first, VectorIterator const &second) {
        return first.p - second.p;
    }

    friend VectorIterator operator+(VectorIterator that, size_t n) {
        that += n;
        return that;
    }

    friend VectorIterator operator-(VectorIterator that, size_t n) {
        that -= n;
        return that;
    }

    friend VectorIterator operator+(size_t n, VectorIterator const &that) {
        that += n;
        return that;
    }

private:
    template<typename> friend
    struct MyVector;
    template<typename> friend
    struct const_VectorIterator;
    pointer p = nullptr;

    explicit VectorIterator(pointer ptr) : p(ptr) {}
};

template<typename T>
struct const_VectorIterator {
    typedef std::random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T const *pointer;
    typedef T const &reference;

    const_VectorIterator() = default;

    ~const_VectorIterator() = default;

    const_VectorIterator(const_VectorIterator const &) = default;

    const_VectorIterator(VectorIterator<T> const &that) : p(that.p) {}

    const_VectorIterator &operator=(const_VectorIterator const &) = default;

    const_VectorIterator &operator++() {
        ++p;
        return *this;
    }

    const const_VectorIterator operator++(int) {
        VectorIterator tmp(*this);
        ++*this;
        return tmp;
    }

    const_VectorIterator &operator--() {
        --p;
        return *this;
    }

    const const_VectorIterator operator--(int) {
        const_VectorIterator tmp(*this);
        --*this;
        return tmp;
    }

    reference operator[](size_t i) {
        return p[i];
    }

    reference operator*() const {
        return *p;
    }

    pointer operator->() const {
        return p;
    }

    const_VectorIterator &operator+=(size_t n) {
        p += n;
        return *this;
    }

    const_VectorIterator &operator-=(size_t n) {
        p -= n;
        return *this;
    }

    friend difference_type operator-(const_VectorIterator const &in1, const_VectorIterator const &in2) {
        return in1.p - in2.p;
    }

    friend const_VectorIterator operator+(const_VectorIterator that, size_t n) {
        that += n;
        return that;
    }

    friend const_VectorIterator operator-(const_VectorIterator that, size_t n) {
        that -= n;
        return that;
    }

    friend const_VectorIterator operator+(size_t n, const_VectorIterator that) {
        that += n;
        return that;
    }

    bool operator==(const_VectorIterator const &that) const {
        return p == that.p;
    }

    bool operator!=(const_VectorIterator const &that) const {
        return p != that.p;
    }

    bool operator<(const_VectorIterator const &that) {
        return p < that.p;
    }

    bool operator>(const_VectorIterator const &that) {
        return p > that.p;
    }

    bool operator<=(const_VectorIterator const &that) {
        return p <= that.p;
    }

    bool operator>=(const_VectorIterator const &that) {
        return p >= that.p;
    }

private:
    template<typename> friend
    struct MyVector;

    explicit const_VectorIterator(pointer p) : p(p) {}

    pointer p = nullptr;
};

template<class T>
struct MyVector {
    typedef VectorIterator<T> iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef const_VectorIterator<T> const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    MyVector() noexcept = default;

    ~MyVector() noexcept {
        clear();
    }

    bool empty() const noexcept {
        return ((DATA.index() == 0) && ((show_BO() == nullptr) || (data_size() == 0)));
    }

    size_t size() const noexcept {
        if (DATA.index() == 0) {
            if (show_BO() == nullptr) {
                return 0;
            } else {
                return data_size();
            }
        } else {
            return 1;
        }
    }

    T const &operator[](size_t i) const {
        if (is_big()) { // pre : 0 <= i < size()
            assert(i < data_size());
            return *(data_begin() + i);
        } else {
            assert(i == 0);
            return std::get<1>(DATA);
        }
    }

    T &operator[](size_t i) {
        selfdetach(); // pre : 0 <= i < size()
        if (is_big()) {
            assert(i < data_size());
            return *(data_begin() + i);
        } else {
            assert(i == 0);
            return std::get<1>(DATA);
        }
    }

    T const &back() const noexcept {
        assert(!empty()); // pre: MyVector is not empty
        return is_big() ? *(data_begin() + data_size() - 1) : std::get<1>(DATA);
    }

    T &back() {
        assert(!empty()); // pre: MyVector is not empty
        selfdetach();
        return is_big() ? *(data_begin() + data_size() - 1) : std::get<1>(DATA);
    }

    T const &front() const noexcept {
        assert(!empty()); // pre: MyVector is not empty
        return is_big() ? (*data_begin()) : std::get<1>(DATA);
    }

    T &front() {
        assert(!empty()); // pre: MyVector is not empty
        selfdetach();
        return is_big() ? (*data_begin()) : std::get<1>(DATA);
    }

    MyVector operator=(MyVector const &that) {
        if (this == &that) {
            return *this;
        }
        clear();
        DATA = that.DATA;
        if (is_big()) {
            ++show_BO()->cnt;
        }
        return *this;
    }

    MyVector(MyVector const &that) : MyVector() {
        if (that.is_big()) {
            DATA = that.show_BO();
            ++show_BO()->cnt;
        } else if (!that.empty()) {
            push_back(that[0]);
        }
    }

    void clear() { // clears MyVector correctly
        if (is_big()) {
            if (data_links() == 0) {
                destroy_BO();
            } else {
                --show_BO()->cnt;
            }
        }
        DATA = nullptr;
    }

    T *data() {
        selfdetach();
        if (empty()) {
            return nullptr;
        }
        return is_big() ? data_begin() : &(std::get<1>(DATA));
    }

    const T *data() const {
        if (empty()) {
            return nullptr;
        }
        return is_big() ? data_begin() : &(std::get<1>(DATA));
    }

    void reserve(size_t n) {
        if (empty()) {
            BigObj *new_BO = init_BO(n + 10);
            DATA = new_BO;
            return;
        }
        if (n == 0) { return; }
        if (is_big()) {
            BigObj *new_BO = copy_BO(n + data_capacity());
            destroy_BO();
            DATA = new_BO;
        } else {
            BigObj *new_BO = init_BO(n + 10);
            new_BO->sz = 1;
            try {
                copy_construct(new_BO->ptr, std::get<1>(DATA));
            } catch (...) {
                dealloc_BO(new_BO);
                throw;
            }
            DATA = new_BO;
        }
    }

    void pop_back() {
        assert(!empty()); // pre: MyVector is not empty
        selfdetach();
        if (is_big()) {
            std::destroy_at(data_begin() + data_size() - 1);
            --show_BO()->sz;
            if (data_size() == 0) {
                clear();
            }
        } else {
            DATA = nullptr;
        }
    }

    void push_back(T const &x) {
        selfdetach();
        if (empty() && (show_BO() == nullptr)) {
            try {
                DATA = x;
            } catch (...) {
                DATA = nullptr;
                throw;
            }
        } else {
            if (is_big()) {
                push_back_big(x);
            } else {
                BigObj *new_BO = init_BO(10);
                new_BO->sz = 2;
                try {
                    copy_construct(new_BO->ptr + 1, x);
                } catch (...) {
                    dealloc_BO(new_BO);
                    throw;
                }
                try {
                    copy_construct(new_BO->ptr, std::get<1>(DATA));
                } catch (...) {
                    std::destroy_at(new_BO->ptr + 1);
                    dealloc_BO(new_BO);
                    throw;
                }
                DATA = new_BO;
            }
        }
    }

    iterator begin() {
        selfdetach();
        if (!empty()) {
            return is_big() ? iterator(data_begin()) : iterator(&(std::get<1>(DATA)));
        } else {
            return iterator(nullptr);
        }
    }

    iterator end() {
        selfdetach();
        if (!empty()) {
            return is_big() ? iterator(data_begin() + data_size()) : iterator(&(std::get<1>(DATA)) + 1);
        } else {
            return iterator(nullptr);
        }
    }

    const_iterator begin() const noexcept {
        if (!empty()) {
            return is_big() ? const_iterator(data_begin()) : const_iterator(&(std::get<1>(DATA)));
        } else {
            return const_iterator(nullptr);
        }
    }

    const_iterator end() const noexcept {
        if (!empty()) {
            return is_big() ? const_iterator(data_begin() + data_size()) : const_iterator(&(std::get<1>(DATA)) + 1);
        } else {
            return const_iterator(nullptr);
        }
    }

    const_iterator cbegin() const noexcept { return begin(); }

    const_iterator cend() const noexcept { return end(); }

    reverse_iterator rbegin() {
        selfdetach();
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        selfdetach();
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    const_reverse_iterator crend() const noexcept { return rend(); }

    void insert(const_iterator It, T const &x) {
        selfdetach();
        if (empty() || It == end()) {
            push_back(x);
        } else {
            MyVector new_vec;
            new_vec.reserve(size() + 1);
            auto curr_it = cbegin();
            try {
                while (curr_it < It) {
                    new_vec.push_back(*curr_it);
                    ++curr_it;
                }
                new_vec.push_back(x);
                while (curr_it < cend()) {
                    new_vec.push_back(*curr_it);
                    ++curr_it;
                }
            } catch (...) {
                new_vec.clear();
                throw;
            }
            *this = new_vec;
        }
    }

    void erase(const_iterator It1, const_iterator It2) {
        assert(It1 <= It2); // pre: It1 <= It2
        selfdetach();
        if (It2 == end()) {
            while (It1 != It2) {
                pop_back();
                ++It1;
            }
        } else {
            MyVector new_vec;
            try {
                auto curr_it = cbegin();
                while (curr_it != It1) {
                    new_vec.push_back(*curr_it);
                    ++curr_it;
                }
                curr_it = It2;
                while (curr_it != end()) {
                    new_vec.push_back(*curr_it);
                    ++curr_it;
                }
            } catch (...) {
                new_vec.clear();
                throw;
            }
            *this = new_vec;
        }
    }

    void erase(const_iterator It1) {
        erase(It1, It1 + 1);
    }
    // basic
    void swap(MyVector &that) {
        bool is_T_this = (DATA.index() == 1);
        bool is_T_that = (that.DATA.index() == 1);
        if (is_T_that && is_T_this) {
            std::swap(DATA, that.DATA);
        } else if (!is_T_that && !is_T_this) {
            try {
                std::swap(DATA, that.DATA);
            } catch (...) {
                DATA = nullptr;
                that.DATA = nullptr;
                throw;
            }
        } else if (!is_T_that && is_T_this) {
            that.swap(*this);
        } else {
            BigObj *old_BO = show_BO();
            try {
                DATA = that.DATA;
            } catch (...) {
                that.DATA = nullptr;
                DATA = old_BO;
                throw;
            }
            that.DATA = old_BO;
        }
    }

    template<typename T1>
    friend bool operator<(MyVector<T1> const &, MyVector<T1> const &) noexcept;

    template<typename T1>
    friend bool operator>(MyVector<T1> const &, MyVector<T1> const &) noexcept;

    template<typename T1>
    friend bool operator==(MyVector<T1> const &, MyVector<T1> const &) noexcept;

    template<typename T1>
    friend bool operator!=(MyVector<T1> const &, MyVector<T1> const &) noexcept;

    template<typename T1>
    friend bool operator<=(MyVector<T1> const &, MyVector<T1> const &) noexcept;

    template<typename T1>
    friend bool operator>=(MyVector<T1> const &, MyVector<T1> const &) noexcept;

private:
    struct BigObj {
        size_t cnt = 0, sz = 0, capacity = 0;
        T *ptr = nullptr;
    };

    void *alloc_BO(size_t need_sz) { // allocates memory for BigObj
        return operator new(need_sz);
    }

    BigObj *init_BO(size_t cap) { // initializes new BigObj correctly
        auto bo = static_cast<BigObj *>(alloc_BO(sizeof(T) * cap + sizeof(BigObj)));
        bo->capacity = cap;
        bo->sz = bo->cnt = 0;
        bo->ptr = reinterpret_cast<T *>(&bo->ptr + 1);
        return bo;
    }

    void dealloc_BO(BigObj *bo) { // deletes BigObj from the pointer
        operator delete(static_cast<void *>(bo));
    }

    void destroy_BO() { // destroys BigObject correctly
        assert(DATA.index() == 0);  // pre: DATA holds BigObj
        std::destroy(data_begin(), data_begin() + data_size());
        dealloc_BO(std::get<0>(DATA));
        std::get<0>(DATA) = nullptr;
    }

    void copy_construct(T *p, T *c) { // like std::allocator construct but copying instead of moving
        assert(p != nullptr && c != nullptr); // pre: p, c != nullptr
        new(p) T(*c);
    }

    template<typename ...Args>
    void copy_construct(T *p, Args const &... args) {
        assert(p != nullptr); // pre: p != nullptr
        new(p) T(args...);
    }

    BigObj *copy_BO(size_t new_cap) { // safely copies BigObj (similar logic with std::uninitialized_copy)
        BigObj *newBO = init_BO(new_cap);
        size_t pos = 0;
        size_t new_sz = std::min(new_cap, data_size());
        try {
            for (; pos < new_sz; ++pos) {
                copy_construct(newBO->ptr + pos, data_begin() + pos);
            }
        } catch (...) {
            std::destroy(newBO->ptr, newBO->ptr + pos);
            dealloc_BO(newBO);
            throw;
        }
        newBO->sz = new_sz;
        return newBO;
    }

    void selfdetach() { // detaches BigObject if necessary
        if (DATA.index() == 0 && show_BO() != nullptr && data_links() > 0) {
            BigObj *new_BO = copy_BO(data_capacity());
            --show_BO()->cnt;
            DATA = new_BO;
        }
    }

    std::variant<BigObj *, T> DATA;

    BigObj *show_BO() const noexcept { // returns a pointer to BigObject
        assert(DATA.index() == 0); // pre: DATA holds BigObj
        return std::get<0>(DATA);
    }

    size_t data_links() const noexcept { // returns the number of pointers to current BigObject
        assert(show_BO() != nullptr); // pre: BigObj is not NULL;
        return show_BO()->cnt;
    }

    size_t data_size() const noexcept { // returns the size of current BigObject
        assert(show_BO() != nullptr); // pre: BigObj is not NULL;
        return show_BO()->sz;
    }

    size_t data_capacity() const noexcept { // returns the capacity of current BigObject
        assert(show_BO() != nullptr); // pre: BigObj is not NULL;
        return show_BO()->capacity;
    }

    T *data_begin() const noexcept { // returns a pointer to current BigObject data
        assert(show_BO() != nullptr); // pre: BigObj is not NULL;
        return show_BO()->ptr;
    }

    bool is_big() const noexcept { // checks if MyVector's state is BigObj
        return (DATA.index() == 0 && show_BO() != nullptr);
    }

    void push_back_big(T const &x) { // safe push_back with expanding
        assert(is_big()); // pre: DATA holds valid BigObj
        size_t pos = data_size();
        if (data_size() == data_capacity()) {
            BigObj *new_BO = copy_BO(data_capacity() * 2 + 1);
            try {
                copy_construct(new_BO->ptr + pos, x);
            } catch (...) {
                std::destroy(new_BO->ptr, new_BO->ptr + pos);
                dealloc_BO(new_BO);
                throw;
            }
            destroy_BO();
            DATA = new_BO;
        } else {
            copy_construct(data_begin() + pos, x);
        }
        ++show_BO()->sz;
    }

    static_assert((sizeof(DATA)) <= (sizeof(void *) + std::max(sizeof(T), sizeof(void *))));
};

template<typename T1>
bool operator<(const MyVector<T1> &first, const MyVector<T1> &second) noexcept {
    size_t sz = std::min(first.size(), second.size());
    for (size_t i = 0; i < sz; ++i) {
        if (first[i] != second[i]) {
            return (first[i] < second[i]);
        }
    }
    return first.size() < second.size();
}

template<typename T1>
bool operator>(const MyVector<T1> &first, const MyVector<T1> &second) noexcept {
    return second < first;
}

template<typename T1>
bool operator==(const MyVector<T1> &first, const MyVector<T1> &second) noexcept {
    return !(first < second || second < first);
}

template<typename T1>
bool operator!=(const MyVector<T1> &first, const MyVector<T1> &second) noexcept {
    return !(first == second);
}

template<typename T1>
bool operator<=(const MyVector<T1> &first, const MyVector<T1> &second) noexcept {
    return !(first > second);
}

template<typename T1>
bool operator>=(const MyVector<T1> &first, const MyVector<T1> &second) noexcept {
    return !(first < second);
}

template<typename T1>
void swap(MyVector<T1> &first, MyVector<T1> &second) {
    first.swap(second);
}

#endif //BIGINT_MY_VECTOR_H
