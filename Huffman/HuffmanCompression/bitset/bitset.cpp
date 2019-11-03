//
// Created by daniil on 15.06.19.
//

#include "bitset.hpp"
#include "../consts.h"

BitSet::BitSet() : V(), end_pos(0) {}

BitSet::BitSet(uint8_t b) : V({b}), end_pos(0) {}

bool BitSet::empty() {
    return V.empty();
}

void BitSet::push_back(bool b) {
    if (end_pos == 0) {
        V.push_back(0);
    }
    byte left = consts::MAX_POS - end_pos;
    if (b == 0) {
        V.back() &= (consts::MAX_BYTE ^ (1u << (left)));
    } else {
        V.back() |= ((byte) b << (left));
    }
    end_pos = (uint8_t) ((end_pos + 1u) % consts::BYTE_SIZE);
}

void BitSet::pop_back() {
    if (end_pos == 1) {
        V.pop_back();
        end_pos = 0;
    } else if (end_pos == 0) {
        end_pos = consts::MAX_POS;
    } else {
        --end_pos;
    }
}

size_t BitSet::get_size() const {
    size_t res = V.size() * consts::BYTE_SIZE;
    if (end_pos > 0) {
        res -= (consts::BYTE_SIZE - end_pos);
    }
    return res;
}

bool BitSet::at(size_t index) const {
    size_t byte_num = (index / consts::BYTE_SIZE);
    auto pos_in_byte = (uint8_t)(index % consts::BYTE_SIZE);
    bool res = (V[byte_num] & (1u << (consts::MAX_POS - pos_in_byte))) > 0;
    return res;
}

void BitSet::multipop(size_t sz) {
    for (size_t i = 0; i < sz; ++i) {
        pop_back();
    }
}

void BitSet::concat(BitSet const &that) {
    size_t bit_sz = that.get_size();
    if (end_pos == 0) {
        size_t old_sz = V.size();
        V.resize(old_sz + that.V.size());
        std::copy(that.V.begin(), that.V.end(), V.begin() + old_sz);
        end_pos = that.end_pos;
    } else {
        for (size_t i = 0; i < bit_sz; ++i) {
            push_back(that.at(i));
        }
    }
}

std::string BitSet::to_string() const {
    std::string res;
    size_t sz = get_size();
    for (size_t i = 0; i < sz; ++i) {
        res += std::to_string(static_cast<char>(at(i)));
    }
    return res;
}

std::vector<uint8_t> const &BitSet::show_data() const {
    return V;
}
