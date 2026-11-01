#include <string>
#include "ascii_encode.h"

// Each byte as its decimal ASCII code, space-separated: "Hi" -> "72 105"
std::string ascii_encode(const std::string& value) {
    std::string encoded;

    for (unsigned char byte : value) {
        if (!encoded.empty()) {
            encoded += ' ';
        }
        encoded += std::to_string(byte);
    }

    return encoded;
}

