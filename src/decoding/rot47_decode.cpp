#include <string>
#include "rot47_decode.h"

// ROT47 is its own inverse: rotating printable ASCII ('!'..'~') by 47 places restores the original text.
std::string rot47_decode(const std::string& value) {
    std::string decoded = value;

    for (char& c : decoded) {
        if (c >= '!' && c <= '~') {
            c = static_cast<char>('!' + (c - '!' + 47) % 94);
        }
    }

    return decoded;
}

