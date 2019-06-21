//
// Created by daniil on 15.06.19.
//

#ifndef ELDERLY_HUFFMAN_BITSET_H
#define ELDERLY_HUFFMAN_BITSET_H

#include <cstdint>
#include <vector>
#include <string>

struct BitSet {
    typedef uint8_t byte;
    BitSet();
    explicit BitSet(byte b);
    void push_back(bool);
    void pop_back();
    void multipop(size_t sz);
    size_t get_size() const;
    bool at(size_t ind) const;
    void concat(BitSet const&);
    bool empty();
    std::string to_string() const;
    std::vector<uint8_t> const& show_data() const;
private:
    std::vector<uint8_t> V;
    uint8_t end_pos;
};
#endif //ELDERLY_HUFFMAN_BITSET_H
