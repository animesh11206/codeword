#include <cstdint>
#include <string>
#include "ascii_decode.h"

// Decodes space-separated decimal byte values ("72 105" -> "Hi").
std::string ascii_decode(const std::string& value) {
    std::string decoded;
    std::uint32_t current = 0;
    bool in_number = false;

    for (unsigned char c : value) {
        if (c >= '0' && c <= '9') {
            current = current * 10 + (c - '0');
            in_number = true;
        }
        else if (in_number) {
            decoded += static_cast<char>(current & 0xFF);
            current = 0;
            in_number = false;
        }
    }

    if (in_number) {
        decoded += static_cast<char>(current & 0xFF);
    }

    return decoded;
}

