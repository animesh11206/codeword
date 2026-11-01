#include <cstdint>
#include <string>
#include "octal_decode.h"

// Decodes space-separated octal byte values ("110 151" -> "Hi").
std::string octal_decode(const std::string& value) {
    std::string decoded;
    std::uint32_t current = 0;
    bool in_number = false;

    for (unsigned char c : value) {
        if (c >= '0' && c <= '7') {
            current = (current << 3) | static_cast<std::uint32_t>(c - '0');
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

