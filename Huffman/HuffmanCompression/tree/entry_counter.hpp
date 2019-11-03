//
// Created by daniil on 14.06.19.
//

#ifndef ELDERLY_HUFFMAN_ENTRY_COUNTER_H
#define ELDERLY_HUFFMAN_ENTRY_COUNTER_H

#include <vector>
#include <array>
#include "../consts.h"

struct EntryCounter {
    EntryCounter() : entries() {
        entries.fill(0);
    };
    ~EntryCounter() = default;
    std::array<uint32_t, consts::ALPH_SIZE> get_res() const {
        return entries;
    }

    template<typename It>
    void update(It i1, It i2) {
        for (auto t = i1; t != i2; ++t) {
            ++entries[*t & consts::MAX_BYTE];
        }
    }
private:
    std::array<uint32_t, consts::ALPH_SIZE> entries;
};
#endif //ELDERLY_HUFFMAN_ENTRY_COUNTER_H
