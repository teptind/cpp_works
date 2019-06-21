//
// Created by daniil on 21.06.19.
//

#ifndef ELDERLY_HUFFMAN_FILE_USING_H
#define ELDERLY_HUFFMAN_FILE_USING_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "../HuffmanCompression/consts.h"
#include "../HuffmanCompression/tree/entry_counter.hpp"
#include "../HuffmanCompression/tree/trees.h"

void encoding(std::string const &input_name, std::string const &output_name) {
    try {
        std::ifstream in;
        in.open(input_name);
        if (!in.is_open()) {
            std::cout << "unable to open file " << input_name << " to read\n";
            return;
        }
        size_t input_sz = 0;
        std::vector<char> buffer(consts::BUFF_SIZE);
        EntryCounter counter;
        while (!in.eof()) {
            in.read(buffer.data(), buffer.size());
            counter.update(buffer.begin(), buffer.begin() + in.gcount());
            input_sz += in.gcount();
        }
        in.clear();
        in.seekg(0);

        TreeEncoder treeEncoder(counter);
        std::string serve_source = treeEncoder.get_serve_code();

        std::ofstream out;
        out.open(output_name, std::ios::out);
        if (!out.is_open()) {
            in.close();
            std::cout << "unable to open file " << output_name << " to write\n";
            return;
        }
        size_t output_sz = serve_source.size();
        out.write(serve_source.data(), serve_source.size());
        while (!in.eof()) {
            in.read(buffer.data(), buffer.size());
            std::string encoded_block = treeEncoder.encode_block(buffer.begin(), buffer.begin() + in.gcount());
            out.write(encoded_block.data(), encoded_block.size());
            output_sz += encoded_block.size();
        }
        std::cout << "successfully_encoded " << input_name << " to " << output_name << ":\n";
        std::cout << "inputsz: " << input_sz << "\n";
        std::cout << "outputsz: " << output_sz << "\n";
        in.close();
        out.close();
    } catch (std::runtime_error &re) {
        std::cout << "unable to encode file " << output_name << ". Message: " << re.what() << std::endl;
    }
}

void decoding(std::string const &input_name, std::string const &output_name) {
    try {
        std::ifstream in;
        std::ofstream out;
        in.open(input_name, std::ios::in);
        if (!in.is_open()) {
            std::cout << "unable to open file " << input_name << " to read\n";
            return;
        }
        out.open(output_name, std::ios::out);
        if (!out.is_open()) {
            in.close();
            std::cout << "unable to open file " << output_name << " to write\n";
            return;
        }
        size_t input_sz = 0;
        size_t output_sz = 0;
        std::vector<char> buffer(consts::BUFF_SIZE);
        TreeDecoder treeDecoder;
        while (!in.eof()) {
            in.read(buffer.data(), buffer.size());
            std::string decoded_block = treeDecoder.get_code(buffer.begin(), buffer.begin() + in.gcount());
            input_sz += in.gcount();
            out.write(decoded_block.data(), decoded_block.size());
            output_sz += decoded_block.size();
        }
        std::cout << "successfully_decoded " << input_name << " to " << output_name << ":\n";
        std::cout << "inputsz: " << input_sz << "\n";
        std::cout << "outputsz: " << output_sz << "\n";
    } catch (std::runtime_error &re) {
        std::cout << "unable to decode file " << output_name << ". Message: " << re.what() << std::endl;
    }
}

bool same_files(std::string const &input_first, std::string const &input_second) {
    std::ifstream in1, in2;
    in1.open(input_first, std::ios::in);
    if (!in1.is_open()) {
        std::cout << "unable to open file " << input_first << " to read\n";
        return false;
    }
    in2.open(input_second, std::ios::in);
    if (!in2.is_open()) {
        std::cout << "unable to open file " << input_second << " to read\n";
        return false;
    }
    std::vector<char> first_data(consts::BUFF_SIZE), second_data(consts::BUFF_SIZE);
    while (!in1.eof() && !in2.eof()) {
        in1.read(first_data.data(), consts::BUFF_SIZE);
        in2.read(second_data.data(), consts::BUFF_SIZE);
        auto was_read_first = in1.gcount();
        auto was_read_second = in2.gcount();
        if (was_read_first != was_read_second) {
            return false;
        }
        for (size_t i = 0; i < was_read_first; i++) {
            if (first_data[i] != second_data[i]) {
                return false;
            }
        }
    }
    return in1.eof() && in2.eof();
}


void make_random(std::string const &file_name, size_t cnt) {
    srand(time(NULL));
    std::ofstream out;
    out.open(file_name, std::ios::out);
    if (!out.is_open()) {
        out.close();
        std::cout << "unable to open file " << file_name << " to write\n";
        return;
    }
    std::string s;
    for (size_t i = 0; i < cnt; i++) {
        s += (char)((rand() % 128 * 2) - 128);
    }
    out.write(s.data(), s.size());
    out.close();
}
void file_clear(std::string const &file_name) {
    std::ofstream out;
    out.open(file_name, std::ios::out);
    if (!out.is_open()) {
        out.close();
        std::cout << "unable to open file " << file_name << " to write\n";
        return;
    }
    std::string s;
    out.write(s.data(), s.size());
    out.close();
}
#endif //ELDERLY_HUFFMAN_FILE_USING_H
