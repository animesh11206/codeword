#include <string>
#include "octal_escape_encode.h"

// Each byte as a C-style octal escape: "Hi" -> "\110\151"
std::string octal_escape_encode(const std::string& value) {
    std::string encoded;

    for (unsigned char byte : value) {
        encoded += '\\';
        encoded += static_cast<char>('0' + ((byte >> 6) & 0x7));
        encoded += static_cast<char>('0' + ((byte >> 3) & 0x7));
        encoded += static_cast<char>('0' + (byte & 0x7));
    }

    return encoded;
}

