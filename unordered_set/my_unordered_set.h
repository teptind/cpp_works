//
// Created by teptind on 14.09.2019.
//

#ifndef MY_UNORDERED_SET_MY_UNORDERED_SET_H
#define MY_UNORDERED_SET_MY_UNORDERED_SET_H

#include <iostream>
#include <vector>
#include <vector>
#include <cassert>

template<typename T>
struct my_unordered_set {
    struct const_uset_iterator {
        typedef std::forward_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T const *pointer;
        typedef T const &reference;

        const_uset_iterator() = default;

        ~const_uset_iterator() = default;

        const_uset_iterator &operator=(const_uset_iterator const &) = default;

        explicit const_uset_iterator(std::vector<std::vector<T> > const &s,
                                     size_t bucket_id,
                                     size_t elem_id) :
                s(s), bucket_id(bucket_id), elem_id(elem_id) {}

        const_uset_iterator(const_uset_iterator const &) = default;

        const_uset_iterator &operator++() {
            assert(s[bucket_id].size() != elem_id);
            elem_id++;
            if (elem_id == s[bucket_id].size()) {
                do {
                    bucket_id++;
                } while (bucket_id < s.size() && s[bucket_id].empty());
                if (bucket_id != s.size()) {
                    elem_id = 0;
                } else {
                    bucket_id--;
                    elem_id = s[bucket_id].size();
                }
            }
            return (*this);
        }

        const_uset_iterator operator++(int) {
            const_uset_iterator res(*this);
            ++(*this);
            return res;
        }

        friend bool operator==(const_uset_iterator const &first, const_uset_iterator const &second) {
            return (first.s == second.s && first.bucket_id == second.bucket_id && first.elem_id == second.elem_id);
        }

        friend bool operator!=(const_uset_iterator const &first, const_uset_iterator const &second) {
            return !(first == second);
        }

        reference operator*() const {
            return s[bucket_id][elem_id];
        }

        pointer operator->() const {
            return &s[bucket_id][elem_id];
        }

    private:
        template<typename> friend
        struct my_unordered_set;
        std::vector<std::vector<T> > const &s;
        size_t bucket_id{}, elem_id{};
    };

    typedef const_uset_iterator iterator;
    typedef const_uset_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    my_unordered_set() : sz(10), cnt(0), table(sz, std::vector<T>()) {}

    explicit my_unordered_set(size_t sz) : sz(sz), cnt(0), table(sz, std::vector<T>()) {}

    ~my_unordered_set() = default;

    my_unordered_set(my_unordered_set const &that) : sz(that.sz), cnt(that.cnt), table(that.table) {}

    my_unordered_set &operator=(my_unordered_set const &that) = default;

    void clear() {
        table.clear();
        table.resize(sz);
        cnt = 0;
    }

    bool empty() const {
        return (cnt == 0);
    }

    const_uset_iterator begin() const {
        for (size_t it = 0; it < table.size(); ++it) {
            if (!table[it].empty()) {
                return const_uset_iterator(table, it, 0);
            }
        }
        return end();
    }

    const_iterator end() const {
        return const_uset_iterator(table, table.size() - 1, table.back().size());
    }

    const_iterator find(T const &x) const {
        size_t pos = std::hash<T>()(x) % sz;
        for (size_t it = 0; it < table[pos].size(); ++it) {
            if (table[pos][it] == x) {
                return const_uset_iterator(table, pos, it);
            }
        }
        return end();
    }

    std::pair<const_iterator, bool> insert(T const &x) {
        const_iterator curr = find(x);
        if (curr != end()) {
            return {curr, 0};
        }
        size_t pos = std::hash<T>()(x) % sz;
        table[pos].push_back(x);
        cnt++;
        expand_if_nessesary();
        const_uset_iterator res(table, pos, table[pos].size() - 1);
        return {res, 1};
    }

    void erase(T const &x) {
        const_iterator curr = find(x);
        if (curr == end()) { return; }
        table[curr.bucket_id].erase(table[curr.bucket_id].begin() + curr.elem_id);
        cnt--;
    }


private:
    size_t sz, cnt;
    std::vector<std::vector<T> > table;

    void expand_if_nessesary() {
        if (4 * sz <= 3 * cnt) {
            my_unordered_set new_set(sz * 2);
            for (size_t i = 0; i < table.size(); i++) {
                for (auto j = table[i].begin(); j != table[i].end(); j++) {
                    new_set.insert(*j);
                }
            }
            std::swap(*this, new_set);
        }
    }
};


#endif //MY_UNORDERED_SET_MY_UNORDERED_SET_H
