#include <string>
#include "rot13_decode.h"

// ROT13 is its own inverse: rotating ASCII letters by 13 places restores the original text.
std::string rot13_decode(const std::string& value) {
    std::string decoded = value;

    for (char& c : decoded) {
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>('a' + (c - 'a' + 13) % 26);
        }
        else if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>('A' + (c - 'A' + 13) % 26);
        }
    }

    return decoded;
}

