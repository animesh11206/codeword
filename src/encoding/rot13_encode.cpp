#include <string>
#include "rot13_encode.h"

// Rotates ASCII letters by 13 places; everything else is unchanged.
std::string rot13_encode(const std::string& value) {
    std::string encoded = value;

    for (char& c : encoded) {
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>('a' + (c - 'a' + 13) % 26);
        }
        else if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>('A' + (c - 'A' + 13) % 26);
        }
    }

    return encoded;
}

