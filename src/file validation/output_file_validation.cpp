#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include "output_file_validation.h"

bool Output_File_Validation (const std::string& output_file) {

    try {

        std::error_code error_code;
        std::string input;
        std::filesystem::path output_path(output_file);

        if (output_path.extension() != ".txt") {
            std::cerr << "Error: Output file '" << output_file << "' must have a '.txt' extension." << std::endl;
            return false;
        }

        if (!std::filesystem::is_regular_file(output_file, error_code)) {

            if (error_code) {

                if (error_code == std::errc::no_such_file_or_directory) {
                    std::cout << "The path '" << output_file << "' does not exist. Would you like to create it? (y/n): ";
                    std::cin >> input;

                    if (input == "y" || input == "Y") {
                        std::filesystem::path file_path(output_file);

                        if (file_path.has_parent_path()) {
                            std::filesystem::create_directories(file_path.parent_path(), error_code);
                            if (error_code) {
                                std::cerr << "Error creating directories for '" << output_file 
                                          << "': " << error_code.message() << '\n';
                                return false;
                            }
                        }

                        std::ofstream new_file(output_file, std::ios::out | std::ios::binary);
                        if (!new_file.is_open()) {
                            std::cerr << "Error: Unable to create output file '" << output_file << "'." << std::endl;
                            return false;
                        }

                        ::output_file = output_file;
                        return true;
                    }
                    else if (input == "n" || input == "N") {
                        return false;
                    }
                    else {
                        std::cerr << "Invalid choice. Please enter y or n." << std::endl;
                        return false;
                    }
                }

                else {
                    std::cerr << "Error accessing '" << output_file << "': " << error_code.message() << std::endl;
                    return false;
                }
            }
            else {
                std::cerr << "Error: '" << output_file << "' is a directory or special file, not a regular file." << std::endl;
                return false;
            }
        }

        std::fstream file(output_file, std::ios::in | std::ios::out | std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Error: Unable to open output file '" << output_file << "'." << std::endl;
            return false;
        }

        if (!::input_file.empty()) {
            std::error_code eq_error;
            if (std::filesystem::equivalent(::input_file, output_file, eq_error)) {
                std::cerr << "Error: Output file '" << output_file
                          << "' cannot be the same as input file '" << ::input_file << "'." << std::endl;
                return false;
            }
        }

        if (file.peek() != std::ifstream::traits_type::eof()) {
            
            std::cout << "Output file '" << output_file << "' is not empty. Do you want to append on it? (y/n)." << std::endl;
            
            std::cin >> input;

            if (input == "y" || input == "Y") {
                append_output = true;
                ::output_file = output_file;
                return true;
            }

            if (input == "n" || input == "N") {
                std::cout << "Cannot write in '" << output_file << "'. Do you want to create a new file?(y/n)" << std::endl;
                std::cin >> input;

                if (input == "y" || input == "Y") {
                    std::cout << "Creating file." << std::endl;
                    output_file_creation = true;
                    ::output_file = output_file;
                    return true;
                }

                if (input == "n" || input == "N") {
                    return false;
                }

                else {
                    std::cerr << "Invalid choice. Please enter y or n." << std::endl;
                    return false;
                }
            }
            else {
                std::cerr << "Invalid choice. Please enter y or n." << std::endl;
                return false;
            }
        }

        ::output_file = output_file;
        return true;
    }

    catch (const std::exception& error_code) {
        std::cerr << "Error: " << error_code.what() << std::endl;
        return false;
    }
}
