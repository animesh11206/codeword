#include <string>
#include "binary_encode.h"

// Each byte as 8 bits, space-separated: "Hi" -> "01001000 01101001"
std::string binary_encode(const std::string& value) {
    std::string encoded;

    for (unsigned char byte : value) {
        if (!encoded.empty()) {
            encoded += ' ';
        }

        for (int bit = 7; bit >= 0; --bit) {
            encoded += ((byte >> bit) & 1) ? '1' : '0';
        }
    }

    return encoded;
}

