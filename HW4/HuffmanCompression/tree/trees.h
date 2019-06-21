//
// Created by daniil on 20.06.19.
//

#ifndef ELDERLY_HUFFMAN_TREES_H
#define ELDERLY_HUFFMAN_TREES_H

#include "entry_counter.hpp"
#include "node.h"
#include "../bitset/bitset.hpp"
#include <string>
#include <vector>
#include <queue>

using std::string;
using std::array;

struct TreeEncoder {
    explicit TreeEncoder(EntryCounter const &counter);
    TreeEncoder() = default;
    ~TreeEncoder() = default;
    string get_serve_code() const;

    template<typename It>
    string encode_block(It begin, It end) const {
        if (ROOT == nullptr) {
            return "";
        }
        string res;
        BitSet curr;
        for (It it = begin; it != end; ++it) {
            curr.concat(symb_code[consts::MAX_BYTE & *it]);
        }
        res += sizet_to_str(curr.get_size());
        std::move(curr.show_data().begin(), curr.show_data().end(), std::back_inserter(res));
        return res;
    }

private:
    std::unique_ptr<Node> ROOT = nullptr;
    BitSet alph_code;
    BitSet tree_code;
    array<BitSet, consts::ALPH_SIZE> symb_code;
    std::vector<std::pair<uint32_t, uint8_t> > table = {};
    void make_code(std::unique_ptr<Node> const &, BitSet &);
    static string sizet_to_str(size_t);
};

/*DECODING_PART*//*DECODING_PART*//*DECODING_PART*//*DECODING_PART*//*DECODING_PART*/
struct TreeDecoder {
    TreeDecoder() = default;
    ~TreeDecoder() = default;

    template<typename It>
    string get_code(It begin, It end) {
        string res;
        while(begin != end) {
            if (decoderState.required_size_bytes > 0 || decoderState.size_to_read == 0) {
                begin = decode_size(begin, end);
            }
            if (decoderState.size_to_read == 0) {
                return res;
            }
            switch (decoderState.curr_act) {
                case ALPH:
                    begin = get_alph(begin, end);
                    if (alph_bytes.size() == decoderState.size_to_read) {
                        decoderState.curr_act = TREE;
                        decoderState.required_size_bytes = consts::SIZET_SIZE;
                        decoderState.size_to_read = 0;
                    } else {
                        return res;
                    }
                    break;
                case TREE:
                    begin = decode_tree(begin, end);
                    if (tree_code.get_size() >= decoderState.size_to_read) {
                        size_t excess_zeros = (8 - decoderState.size_to_read % 8) % 8;
                        tree_code.multipop(excess_zeros);
                        build_tree();
                        decoderState.curr_act = DATA;
                        decoderState.required_size_bytes = consts::SIZET_SIZE;
                        decoderState.size_to_read = 0;
                    } else {
                        return res;
                    }
                    break;
                case DATA:
                    return get_decoded_block(begin, end);
            }
        }
        return res;
    }

private:
    std::unique_ptr<Node> ROOT = nullptr;
    BitSet tree_code;
    std::queue<uint8_t> alph_bytes;
    string decoded_part;

    void build_tree();

    enum act {ALPH, TREE, DATA};
    struct DecoderState {
        Node* curr_v = nullptr;
        act curr_act = ALPH;
        size_t size_to_read = 0;
        uint8_t required_size_bytes = consts::SIZET_SIZE;
    } decoderState;
    
    template<typename It>
    It decode_size(It begin, It end) {
        while (begin != end && decoderState.required_size_bytes) {
            auto curr_bit = (size_t)(consts::MAX_BYTE & *begin) << ((decoderState.required_size_bytes - 1) * 8);
            decoderState.size_to_read |= curr_bit;
            --decoderState.required_size_bytes;
            ++begin;
        }
        return begin;
    }

    template<typename It>
    It get_alph(It begin, It end) {
        while (begin != end && (alph_bytes.size() < decoderState.size_to_read)) {
            alph_bytes.push(*begin);
            ++begin;
        }
        return begin;
    }

    template<typename It>
    It decode_tree(It begin, It end) {
        while(begin != end && (tree_code.get_size() < decoderState.size_to_read)){
            tree_code.concat(BitSet(consts::MAX_BYTE & *begin));
            ++begin;
        }
        return begin;
    }

    void decode_data_block(BitSet const &code);

    template<typename It>
    string const &get_decoded_block(It begin, It end) {
        decoded_part.clear();
        while (begin != end) {
            BitSet data_block;
            if (decoderState.size_to_read == 0 || decoderState.required_size_bytes > 0) {
                begin = decode_size(begin, end);
                if (decoderState.required_size_bytes > 0) {
                    continue;
                }
            }
            while (begin != end && (data_block.get_size() < decoderState.size_to_read)) {
                data_block.concat(BitSet(consts::MAX_BYTE & *begin));
                ++begin;
            }
            if (data_block.get_size() >= decoderState.size_to_read) {
                size_t excess_zeros = (8 - decoderState.size_to_read % 8) % 8;
                data_block.multipop(excess_zeros);

                decoderState.required_size_bytes = consts::SIZET_SIZE;
                decoderState.size_to_read = 0;
            } else {
                decoderState.size_to_read -= data_block.get_size();
            }
            decode_data_block(data_block);
        }
        return decoded_part;
    }
};


#endif //ELDERLY_HUFFMAN_TREES_H
