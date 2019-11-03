//
// Created by daniil on 20.06.19.
//
#include <algorithm>
#include <queue>
#include "trees.h"

/*ENCODING_PART*//*ENCODING_PART*//*ENCODING_PART*//*ENCODING_PART*//*ENCODING_PART*/
TreeEncoder::TreeEncoder(EntryCounter const &counter) {
    auto entries = counter.get_res();
    for (size_t i = 0; i < consts::ALPH_SIZE; ++i) {
        if (entries[i] > 0) {
            table.emplace_back(entries[i], (uint8_t)i);
        }
    }
    std::sort(table.begin(), table.end());
    auto comp = [](Node const* a, Node const* b) { return a->w > b->w; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comp)> Q(comp);
    for (auto p : table) {
        Node *node = new Node(p.first, p.second);
        Q.push(node);
    }
    while (Q.size() > 1) {
        Node* u = Q.top();
        Q.pop();
        Node* v = Q.top();
        Q.pop();
        Node *uv = new Node(u, v);
        Q.push(uv);
    }
    if (!Q.empty()) {
        ROOT.reset(Q.top());
        Q.pop();
        BitSet bitSet;
        make_code(ROOT, bitSet);
    } else {
        ROOT = nullptr;
    }
}

void TreeEncoder::make_code(std::unique_ptr<Node> const &v, BitSet &curr) {
    if (v == nullptr) { return; }
    if (v->is_leaf()) {
        symb_code[v->val] = curr;
        alph_code.concat(BitSet(v->val));
    } else {
        tree_code.push_back(true);
        curr.push_back(true);
        make_code(v->left, curr);
        curr.pop_back();
        tree_code.push_back(false);
        curr.push_back(false);
        make_code(v->right, curr);
        curr.pop_back();
    }
}

string TreeEncoder::sizet_to_str(size_t sz) {
    string res;
    res.resize(consts::SIZET_SIZE);
    for (size_t i = 0; i < consts::SIZET_SIZE; ++i) {
        res[i] = (uint8_t)(sz >> ((consts::SIZET_SIZE - 1 - i) * 8));
    }
    return res;
}

string TreeEncoder::get_serve_code() const {
    if (ROOT == nullptr) {
        return "";
    }
    string res;
    auto alph_data = alph_code.show_data();
    auto tree_data = tree_code.show_data();
    string alph_sz = sizet_to_str(alph_data.size());
    string tree_sz = sizet_to_str(tree_code.get_size());
    res.resize(consts::SIZET_SIZE * 2 + alph_data.size() + tree_data.size());
    auto currInserter = res.begin();

    std::copy(alph_sz.begin(), alph_sz.end(), currInserter);
    currInserter += consts::SIZET_SIZE;

    std::copy(alph_data.begin(), alph_data.end(), currInserter);
    currInserter += alph_data.size();

    std::copy(tree_sz.begin(), tree_sz.end(), currInserter);
    currInserter += consts::SIZET_SIZE;

    std::copy(tree_data.begin(), tree_data.end(), currInserter);
    return res;
}

/*DECODING_PART*//*DECODING_PART*//*DECODING_PART*//*DECODING_PART*//*DECODING_PART*/
void TreeDecoder::build_tree() {  // throws runtime
    if (tree_code.get_size() == 1) {
        ROOT = std::make_unique<Node>(2019, alph_bytes.front());
        alph_bytes.pop();
        return;
    }
    ROOT = std::make_unique<Node>();
    Node *v = ROOT.get();
    size_t tree_sz = tree_code.get_size();
    for (size_t i = 0; i < tree_sz; ++i) {
        if (tree_code.at(i)) {
            v->left = std::make_unique<Node>();
            v->left->parent = v;
            v = v->left.get();
        } else {
            if (alph_bytes.empty()) {
                throw std::runtime_error("incorrect alphabet");
            }
            v->val = alph_bytes.front();
            alph_bytes.pop();
            v = v->parent;
            while (v->parent != nullptr && v->right != nullptr) {
                v = v->parent;
            }
            if (v->right != nullptr) {
                throw std::runtime_error("incorrect tree code");
            } else {
                v->right = std::make_unique<Node>();
                v->right->parent = v;
                v = v->right.get();
            }
        }
    }
    if (v->is_leaf()) {
        if (alph_bytes.empty()) {
            throw std::runtime_error("incorrect alphabet");
        }
        v->val = alph_bytes.front();
        alph_bytes.pop();
    } else {
        throw std::runtime_error("incorrect tree code");
    }
}

void TreeDecoder::decode_data_block(BitSet const &block) {
    size_t block_sz = block.get_size();
    if (tree_code.get_size() == 1) {
        for (size_t i = 0; i < block_sz; ++i) {
            if (block.at(i)) {
                decoded_part += static_cast<char>(ROOT->val);
            } else {
                throw std::runtime_error("incorrect data block");
            }
        }
    } else {
        Node* v = (decoderState.curr_v == nullptr) ? ROOT.get() : decoderState.curr_v;
        for (size_t i = 0; i < block_sz; ++i) {
            if (block.at(i)) {
                v = v->left.get();
            } else {
                v = v->right.get();
            }
            if (v == nullptr) {
                throw std::runtime_error("incorrect data block");
            }
            if (v->is_leaf()) {
                decoded_part += static_cast<char>(v->val);
                v = ROOT.get();
            }
        }
        decoderState.curr_v = v;
    }
}
