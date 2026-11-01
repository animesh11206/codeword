#include <iostream>
#include <string>
#include <fstream>
#include "command_validation.h"
#include "input_file_validation.h"
#include "output_file_validation.h"

bool Command_Validation(int argc, char* argv[]) {

    int operation_count = 0;
    int tmp = 0;

    std::string current_operation;

    bool accepting_operation_flags = false;
    bool wordlist_flag_used = false;
    bool output_flag_used = false;

    if (argc < 2) {
        std::cerr << "Error: No flags or arguments provided." << std::endl;
        return false;
    }

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        bool is_valid_flag = false;
        
        for (int j = 0; j < NUM_ALL_FLAGS; j++) {
            if (arg == ALL_FLAGS[j]) {
                is_valid_flag = true;
                break;
            }
        }

        if (!is_valid_flag) {
            std::cerr << "Error: Invalid flag '" << arg << "'." << std::endl;
            return false;
        }

        if (arg == "-h" || arg == "--help" || arg == "-v" || arg == "--version") {
            if (argc > 2) {
                std::cerr << "Error: Flag '" << arg << "' must be used alone." << std::endl;
                return false;
            }
            return true;
        }

        if (arg == "-w" || arg == "--wordlist") {
            if (wordlist_flag_used) {
                std::cerr << "Error: Flag '" << arg << "' cannot be used more than once." << std::endl;
                return false;
            }
            wordlist_flag_used = true;
            accepting_operation_flags = false;

            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                std::cerr << "Error: Missing file path for '" << arg << "'." << std::endl;
                return false;
            }

            if (!Input_File_Validation(argv[i + 1])) {
                return false;
            }

            ++i; // Skip the file path; it isn't a flag.
            continue;
        }

        if (arg == "-o" || arg == "--output") {
            if (output_flag_used) {
                std::cerr << "Error: Flag '" << arg << "' cannot be used more than once." << std::endl;
                return false;
            }
            output_flag_used = true;
            accepting_operation_flags = false;

            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                std::cerr << "Error: Missing file path for '" << arg << "'." << std::endl;
                return false;
            }

            tmp = i;
            ++i; // Skip the file path; it isn't a flag.
            continue;
        }

        if (arg == "--encoding" || arg == "--decoding" || arg == "--hashing") {
            operation_count++;

            if (operation_count > 1) {
                std::cerr << "Error: Only one operation flag (--encoding, --decoding, or --hashing) can be specified." << std::endl;
                return false;
            }

            current_operation = arg;
            accepting_operation_flags = true;
            continue;
        }

        // Any other flag is an operation sub-flag: must appear directly after the operation flag
        if (current_operation.empty() || !accepting_operation_flags) {
            std::cerr << "Error: Flag '" << arg << "' must be placed directly after --encoding, --decoding, or --hashing." << std::endl;
            return false;
        }

        bool is_matching_subflag = false;

        if (current_operation == "--encoding") {
            for (int j = 0; j < NUM_ENCODING_FLAGS; j++) {
                if (arg == ENCODING_FLAGS[j]) {
                    is_matching_subflag = true;
                    break;
                }
            }

            if (!is_matching_subflag) {
                std::cerr << "Error: Flag '" << arg << "' cannot be used with '" << current_operation << "'." << std::endl;
                return false;
            }

            if (num_operation_flags_used >= max_used_encoding_flags) {
                std::cerr << "Error: Maximum of " << max_used_encoding_flags << " encoding flags exceeded." << std::endl;
                return false;
            }
        }
        else if (current_operation == "--decoding") {
            for (int j = 0; j < NUM_DECODING_FLAGS; j++) {
                if (arg == DECODING_FLAGS[j]) {
                    is_matching_subflag = true;
                    break;
                }
            }

            if (!is_matching_subflag) {
                std::cerr << "Error: Flag '" << arg << "' cannot be used with '" << current_operation << "'." << std::endl;
                return false;
            }

            if (num_operation_flags_used >= max_used_decoding_flags) {
                std::cerr << "Error: Maximum of " << max_used_decoding_flags << " decoding flag exceeded." << std::endl;
                return false;
            }
        }
        else if (current_operation == "--hashing") {
            for (int j = 0; j < NUM_HASHING_FLAGS; j++) {
                if (arg == HASHING_FLAGS[j]) {
                    is_matching_subflag = true;
                    break;
                }
            }

            if (!is_matching_subflag) {
                std::cerr << "Error: Flag '" << arg << "' cannot be used with '" << current_operation << "'." << std::endl;
                return false;
            }

            if (num_operation_flags_used >= max_used_hashing_flags) {
                std::cerr << "Error: Maximum of " << max_used_hashing_flags << " hashing flags exceeded." << std::endl;
                return false;
            }
        }

        used_operation_flags[num_operation_flags_used++] = arg;
    }

    if (!wordlist_flag_used) {
        std::cerr << "Error: Input wordlist is mandatory. Please specify a file using -w or --wordlist." << std::endl;
        return false;
    }

    if (operation_count == 0) {
        std::cerr << "Error: No operation flag specified. Please provide --encoding, --decoding, or --hashing." << std::endl;
        return false;
    }

    if (num_operation_flags_used == 0) {
        std::cerr << "Error: No flags provided after '" << current_operation << "'." << std::endl;
        return false;
    }

    if (output_flag_used && !Output_File_Validation(argv[tmp + 1])) {
        return false;
    }

    return true;
}
