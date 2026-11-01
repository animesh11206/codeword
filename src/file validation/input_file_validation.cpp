#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include "input_file_validation.h"

bool Input_File_Validation (const std::string& input_file) {

    try {

        std::error_code error_code;
        std::filesystem::path file_path(input_file);

        if (file_path.extension() != ".txt") {
            std::cerr << "Error: Input file '" << input_file << "' must have a '.txt' extension." << std::endl;
            return false;
        }

        if (!std::filesystem::is_regular_file(input_file, error_code)) {

            if (error_code) {
                
                std::cerr << "Error accessing '" << input_file << "': " << error_code.message() << std::endl;
                return false;
            }
            else {
                std::cerr << "Error: '" << input_file << "' is a directory or special file, not a regular file." << std::endl;
                return false;
            }
        }

        std::ifstream file(input_file, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Error: Unable to open input file '" << input_file << "'." << std::endl;
            return false;
        }

        if (file.peek() == std::ifstream::traits_type::eof()) {

            if (file.bad()) {
                std::cerr << "Error: I/O failure while reading from '" << input_file << "'." << std::endl;
                return false;
            }
            else {
                std::cerr << "Error: Input file '" << input_file << "' is empty." << std::endl;
                return false;
            }
        } 

        ::input_file = input_file;
        return true;
        
    }

    catch (const std::exception& error_code) {
        std::cerr << "Error: " << error_code.what() << std::endl;
        return false;
    }
}
