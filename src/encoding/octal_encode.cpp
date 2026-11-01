#include <string>
#include "octal_encode.h"

// Each byte as 3-digit octal, space-separated: "Hi" -> "110 151"
std::string octal_encode(const std::string& value) {
    std::string encoded;

    for (unsigned char byte : value) {
        if (!encoded.empty()) {
            encoded += ' ';
        }

        encoded += static_cast<char>('0' + ((byte >> 6) & 0x7));
        encoded += static_cast<char>('0' + ((byte >> 3) & 0x7));
        encoded += static_cast<char>('0' + (byte & 0x7));
    }

    return encoded;
}

