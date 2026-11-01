#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "file_collision_handling.h"

std::string File_Collision_Handling(std::string& file) {

    std::filesystem::path original_path = file; 
    std::filesystem::path new_path = original_path;
    int count = 1;

    while (std::filesystem::exists(new_path)) {
        new_path = original_path.parent_path() / (original_path.stem().string() + "(" + std::to_string(count) + ")" + original_path.extension().string());
        count++;
    }

    return new_path.string();
}