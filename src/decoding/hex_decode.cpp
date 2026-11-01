#include <string>
#include "hex_decode.h"
#include "decoding_utils.h"

// Decodes hex pairs ("4869" -> "Hi").
std::string hex_decode(const std::string& value) {
    std::string decoded;
    int high = -1;

    for (char c : value) {
        int digit = decoding_utils::hex_value(c);
        if (digit < 0) {
            continue;
        }

        if (high < 0) {
            high = digit;
        }
        else {
            decoded += static_cast<char>((high << 4) | digit);
            high = -1;
        }
    }

    return decoded;
}

