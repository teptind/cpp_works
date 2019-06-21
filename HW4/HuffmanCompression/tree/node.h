//
// Created by daniil on 20.06.19.
//

#ifndef ELDERLY_HUFFMAN_NODE_H
#define ELDERLY_HUFFMAN_NODE_H

#include <cstdint>
#include <memory>

struct Node {
    std::unique_ptr<Node> left = nullptr;
    std::unique_ptr<Node> right = nullptr;
    Node *parent = nullptr;
    uint32_t w;
    uint8_t val;

    Node() = default;
    ~Node() = default;
    bool is_leaf() const {
        return (left == nullptr && right == nullptr);
    }
    Node(uint32_t weight, uint8_t s) : w(weight), val(s) {}
    Node(Node *l, Node *r) : left(l), right(r), w(l->w + r->w), val(239) {
        l->parent = this;
        r->parent = this;
    }
};
#endif //ELDERLY_HUFFMAN_NODE_H
