//
// Created by daniil on 20.06.19.
//

#include "gtest/gtest.h"
#include "../HuffmanCompression/bitset/bitset.hpp"
#include "../HuffmanCompression/tree/entry_counter.hpp"
#include "../HuffmanCompression/tree/trees.h"
#include "../utility/file_using.h"

TEST(correctness, Metel) {   // using path from file_using.h to file
    encoding("../tests/examples/Metel", "../tests/examples/buffer_encoded");
    decoding("../tests/examples/buffer_encoded", "../tests/examples/buffer_decoded");
    ASSERT_TRUE(same_files("../tests/examples/buffer_decoded", "../tests/examples/Metel"));
    ASSERT_FALSE(same_files("../tests/examples/random_file", "../tests/examples/buffer_decoded"));
}

TEST(correctness, gtest_comression) {
    encoding("../tests/examples/gtest_as_text", "../tests/examples/buffer_encoded");
    decoding("../tests/examples/buffer_encoded", "../tests/examples/buffer_decoded");
    ASSERT_TRUE(same_files("../tests/examples/buffer_decoded", "../tests/examples/gtest_as_text"));
    ASSERT_FALSE(same_files("../tests/examples/random_file", "../tests/examples/buffer_decoded"));
}
void check_random(size_t sz) {
    make_random("../tests/examples/random_file", sz);
    encoding("../tests/examples/random_file", "../tests/examples/buffer_encoded");
    decoding("../tests/examples/buffer_encoded", "../tests/examples/buffer_decoded");
    ASSERT_TRUE(same_files("../tests/examples/buffer_decoded", "../tests/examples/random_file"));
    file_clear("../tests/examples/buffer_encoded");
    file_clear("../tests/examples/buffer_decoded");
    file_clear("../tests/examples/random_file");
}
TEST(correctness, small_random) {
    check_random(1000);
}
TEST(correctness, normal_random) {
    check_random(100000);
}
TEST(correctness, big_random) {
    check_random(5000000);
}
TEST(correctness, enormous_random) {
    check_random(100000000);
}