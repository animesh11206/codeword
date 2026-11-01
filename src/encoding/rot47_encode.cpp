#include <string>
#include "rot47_encode.h"

// Rotates printable ASCII ('!' to '~') by 47 places; everything else is unchanged.
std::string rot47_encode(const std::string& value) {
    std::string encoded = value;

    for (char& c : encoded) {
        if (c >= '!' && c <= '~') {
            c = static_cast<char>('!' + (c - '!' + 47) % 94);
        }
    }

    return encoded;
}

