//
// Created by daniil on 29.06.19.
//

#ifndef ELDERSET_MY_SET_H
#define ELDERSET_MY_SET_H

#include <bits/unique_ptr.h>
#include <queue>
#include <assert.h>

template <typename T>
struct MySet {
    struct Node {
        Node* left;
        Node* right;
        Node* parent;
        Node() : left(nullptr), right(nullptr), parent(nullptr) {}
        ~Node() = default;
        Node(Node *l, Node *r, Node *p) : left(l), right(r), parent(p) {}
        bool is_leaf() const {
            return (left == nullptr && right == nullptr);
        }
    };
    struct RealNode : Node {
        T value;
        RealNode() = delete;
        ~RealNode() = default;
        explicit RealNode(T const &x) : Node(),  value(x) {}
        RealNode(Node *l, Node *r, Node *p, T const &x) : Node(l, r, p), value(x) {}
        RealNode operator=(Node const&) = delete;
    };
    struct const_SetIterator {
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T const *pointer;
        typedef T const &reference;

        const_SetIterator() = default;
        explicit const_SetIterator(Node* n) : p(n) {}
        const_SetIterator(const_SetIterator const&) = default;
        const_SetIterator& operator=(const_SetIterator const&) = default;

        reference operator*() const {
            return (static_cast<RealNode*>(p)->value);
        }
        pointer operator->() const {
            return &(static_cast<RealNode*>(p)->value);
        }

        friend bool operator ==(const_SetIterator const& first, const_SetIterator const& second) {
            return first.p == second.p;
        }

        friend bool operator !=(const_SetIterator const& first, const_SetIterator const& second) {
            return first.p != second.p;
        }

        const_SetIterator& operator++() {
            assert(p != nullptr);
            if (p->right != nullptr) {
                p = p->right;
                while (p->left != nullptr) {
                    p = p->left;
                }
                return *this;
            } else {
                Node* old;
                do {
                    old = p;
                    p = p->parent;
                } while (p->parent != nullptr && old != p->left);
                return *this;
            }
        }

        const const_SetIterator operator++(int) {
            const_SetIterator res(*this);
            ++(*this);
            return res;
        }

        const_SetIterator& operator--() {
            if (p->left != nullptr) {
                p = p->left;
                while (p->right != nullptr) {
                    p = p->right;
                }
                return *this;
            } else {
                while (p->parent != nullptr && p->parent->left == p) {
                    p = p->parent;
                }
                if (p->parent != nullptr) {
                    p = p->parent;
                }
                return *this;
            }
        }

        const const_SetIterator operator--(int) {
            const_SetIterator res(*this);
            --(*this);
            return res;
        }

    private:
        template<typename> friend
        struct MySet;
        Node* p;
    };
    typedef const_SetIterator iterator;
    typedef const_SetIterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    bool empty() const {
        return FAKE_ROOT.is_leaf();
    }
    const_iterator end() const {
        return const_iterator(&FAKE_ROOT);
    }
    const_iterator begin() const {
        Node* v = &FAKE_ROOT;
        while (v->left != nullptr) {
            v = v->left;
        }
        return const_iterator(v);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin());
    }

    const_iterator cend() const noexcept{
        return end();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    std::pair<const_iterator, bool> insert(T const &x) {
        std::pair<Node*, bool> res = insert_impl(x);
        return {const_iterator(res.first), res.second};
    }
    const_iterator erase(const_iterator that) {
        assert(that != end());
        const_iterator ans(that);
        ++ans;
        Node *curr = that.p;
        if (curr->is_leaf()) {
            if (curr->parent->left == curr) {
                curr->parent->left = nullptr;
            } else {
                curr->parent->right = nullptr;
            }
        } else if (curr->left == nullptr) {
            if (curr == curr->parent->left) {
                curr->parent->left = curr->right;
            } else {
                curr->parent->right = curr->right;
            }
            curr->right->parent = curr->parent;
        } else if (curr->right == nullptr) {
            if (curr == curr->parent->left) {
                curr->parent->left = curr->left;
            } else {
                curr->parent->right =  curr->left;
            }
            curr->left->parent = curr->parent;
        }  else {
            Node* next = ans.p;
            if (next->parent->right == next) {
                if (curr == curr->parent->left) {
                    curr->parent->left = next;
                } else {
                    curr->parent->right = next;
                }
                next->parent = curr->parent;
                next->left = curr->left;
                curr->left->parent = next;
                curr->left = nullptr;
                curr->right = nullptr;
                curr->parent = nullptr;
            } else {
                next->parent->left = next->right;
                if (next->right != nullptr) {
                    next->right->parent = next->parent;
                }
                next->right = nullptr;
                next->parent = nullptr;
                if (curr == curr->parent->left) {
                    curr->parent->left = next;
                } else {
                    curr->parent->right = next;
                }
                next->parent = curr->parent;
                next->right = curr->right;
                next->left = curr->left;
                curr->left->parent = next;
                curr->right->parent = next;
                curr->left = nullptr;
                curr->right = nullptr;
                curr->parent = nullptr;
            }
        }
        delete (static_cast<RealNode*>(curr));
        return ans;
    }
    const_iterator find(T const &x) const {
        const_iterator it = lower_bound(x);
        if (it != end() && *it == x) {
            return it;
        } else {
            return cend();
        }
    }
    const_iterator lower_bound(T const &x) const {
        return const_iterator(lower_bound_impl(x));
    }
    const_iterator upper_bound(T const &x) const {
        const_iterator it = lower_bound(x);
        if (it != end() && *it == x) {
            ++it;
        }
        return it;
    }

    MySet() = default;
    ~MySet() {
        clear();
    }

    MySet(MySet const& that) : MySet() {
        for (auto x = that.cbegin(); x != that.cend(); ++x) {
            insert(*x);
        }
    }

    MySet& operator=(MySet const& that) {
        if (this == &that) {
            return *this;
        }
        MySet x(that);
        swap(x, *this);
        return *this;
    }
    void clear() {
        destroy_node(FAKE_ROOT.left);
        FAKE_ROOT.left = nullptr;
    }

    friend void swap(MySet<T> & first, MySet<T> & second) noexcept {
        if (&first == &second) {
            return;
        }
        std::swap(first.FAKE_ROOT.left, second.FAKE_ROOT.left);
        std::swap(first.FAKE_ROOT.right, second.FAKE_ROOT.right);
        std::swap(first.FAKE_ROOT.parent, second.FAKE_ROOT.parent);
        if (first.FAKE_ROOT.left != nullptr) {
            first.FAKE_ROOT.left->parent = &(first.FAKE_ROOT);
        }
        if (second.FAKE_ROOT.left != nullptr) {
            second.FAKE_ROOT.left->parent = &(second.FAKE_ROOT);
        }
    }

private:
    mutable Node FAKE_ROOT;

    std::pair<Node*, bool> insert_impl(T const& x) {
        if (FAKE_ROOT.left == nullptr) {
            FAKE_ROOT.left = new RealNode(nullptr, nullptr, &FAKE_ROOT, x);
            return {FAKE_ROOT.left, true};
        }
        Node *v = (FAKE_ROOT.left);
        Node *old = v;
        while (v != nullptr) {
            T cur_val(static_cast<RealNode*>(v)->value);
            if (cur_val == x) {
                return {v, false};
            } else if (cur_val > x) {
                old = v;
                v = v->left;
            } else {
                old = v;
                v = v->right;
            }
        }
        T cur_val(static_cast<RealNode*>(old)->value);
        if (cur_val > x) {
            old->left = new RealNode(nullptr, nullptr, old, x);
            return {old->left, true};
        } else {
            old->right = new RealNode(nullptr, nullptr, old, x);
            return {old->right, true};
        }
    }

    Node* lower_bound_impl(T const &x) const {
        if (empty()) {
            return &FAKE_ROOT;
        }
        Node* v = FAKE_ROOT.left;
        Node* res = &FAKE_ROOT;
        T cur_val(static_cast<RealNode*>(v)->value);
        while (v != nullptr) {
            if (cur_val >= x) {
                res = v;
                v = v->left;
            } else {
                v = v->right;
            }
            if (v != nullptr) {
                cur_val = static_cast<RealNode*>(v)->value;
            }
        }
        return res;
    }
    void destroy_node(Node *node) {
        if (node != nullptr) {
            destroy_node(node->left);
            destroy_node(node->right);
        }
        delete(static_cast<RealNode*>(node));
    }
};
#endif //ELDERSET_MY_SET_H
