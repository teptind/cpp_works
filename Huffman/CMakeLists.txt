cmake_minimum_required(VERSION 3.13)
project(Elderly_Huffman)

set(CMAKE_CXX_STANDARD 14)

add_library(huffman_lib STATIC
        HuffmanCompression/tree/entry_counter.hpp
        HuffmanCompression/consts.h
        HuffmanCompression/bitset/bitset.hpp
        HuffmanCompression/bitset/bitset.cpp
        HuffmanCompression/tree/node.h
        HuffmanCompression/tree/trees.h
        HuffmanCompression/tree/trees.cpp)

add_executable(huffman_tests tests/gtest/gtest.h tests/gtest/gtest-all.cc tests/gtest/gtest_main.cc tests/huffman_testing.cpp)
target_link_libraries(huffman_tests huffman_lib)

add_executable(util utility/utility.cpp utility/file_using.h)
target_link_libraries(util huffman_lib)

target_link_libraries(huffman_tests pthread huffman_lib)