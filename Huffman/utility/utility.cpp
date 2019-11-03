#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include "../HuffmanCompression/tree/entry_counter.hpp"
#include "../HuffmanCompression/tree/trees.h"
#include "file_using.h"

int main(int32_t argc, char* argv[]) {
    std::string commands = "\tAvailable commands:\n"
                       "\t\t-encode\tencode a file\n"
                       "\t\t-decode\tdecode a file\n";
    std::string help = "\"Usage: Elderly_Huffman COMMAND SOURCE_FILENAME RESULT_FILENAME\n" + commands;
    std::string inc_msg = "Incorrect arguments. Try \"-h\" for help\n";

    if (argc != 4) {
        if (argc == 2 && std::strcmp(argv[1], "-h") == 0) {
            std::cout << help;
        } else {
            std::cout << inc_msg;
        }
        return 0;
    }
    try {
        if (std::strcmp(argv[1], "-encode") == 0) {
            encoding(std::string(argv[2]), std::string(argv[3]));
        } else if (std::strcmp(argv[1], "-decode") == 0) {
            std::cout << "decoding\n";
            decoding(std::string(argv[2]), std::string(argv[3]));
        } else {
            std::cout << inc_msg;
        }
    } catch (std::runtime_error &re) {
        std::cout << "something went wrong, message: " << re.what() << std::endl;
    }
    return 0;
}

///////////////////-decode ../file.out ../decoded_dich.txt
///////////////////-encode ../file.in ../file.out