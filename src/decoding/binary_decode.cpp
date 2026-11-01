#include <cstdint>
#include <string>
#include "binary_decode.h"

// Decodes 8-bit binary groups (space-separated or continuous) into bytes: "01001000 01101001" -> "Hi".
std::string binary_decode(const std::string& value) {
    std::string decoded;
    std::uint32_t buffer = 0;
    unsigned bits = 0;

    for (unsigned char c : value) {
        if (c == '0' || c == '1') {
            buffer = (buffer << 1) | static_cast<std::uint32_t>(c - '0');
            ++bits;

            if (bits == 8) {
                decoded += static_cast<char>(buffer & 0xFF);
                buffer = 0;
                bits = 0;
            }
        }
        else if (bits > 0) {
            decoded += static_cast<char>(buffer & 0xFF);
            buffer = 0;
            bits = 0;
        }
    }

    if (bits > 0) {
        decoded += static_cast<char>(buffer & 0xFF);
    }

    return decoded;
}

