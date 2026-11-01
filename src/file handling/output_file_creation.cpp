#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include "file_collision_handling.h"
#include "output_file_creation.h"

bool Output_File_Creation(const std::string& input_file, const std::string operations[]) {

    try {
        std::error_code error_code;
        std::filesystem::path current_dir = std::filesystem::current_path();
        std::filesystem::path created_dir = current_dir / "Wordcoder";
        
        bool condition = false;

        if (std::filesystem::is_directory(created_dir, error_code)) {
            condition = true;
        }

        else {
            std::filesystem::create_directory(created_dir, error_code);

            if (error_code) {
                std::cerr << "Error: Cannot create the output path '" << created_dir << "': " << error_code.message() << '\n';
                return false;
            }

            condition = true;
        }

        if (condition == true) {
            std::filesystem::path input_path(input_file);

            std::string file_name = "";
            for (int i = 0; i < 5; i++) {

                if (!operations[i].empty()) {
                    file_name += operations[i] + "_";
                }
            }

            file_name += input_path.filename().string();

            std::string final_path = (created_dir / file_name).string();

            if (std::filesystem::exists(final_path, error_code)) {
                final_path = File_Collision_Handling(final_path);
            }

            std::ofstream new_file(final_path, std::ios::out | std::ios::binary);

            if (!new_file.is_open()) {
                std::cerr << "Error: Unable to create output file '" << final_path << "'." << std::endl;
                return false;
            }

            ::output_file = final_path;
            return true;
        }

        return false;
    }

    catch (const std::exception& error_code) {
        std::cerr << "Error: " << error_code.what() << std::endl;
        return false;
    }

}
