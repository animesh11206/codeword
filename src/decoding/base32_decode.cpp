#include <cstdint>
#include <string>
#include "base32_decode.h"

std::string base32_decode(const std::string& value) {
    std::string decoded;
    std::uint32_t buffer = 0;
    unsigned bits = 0;

    for (unsigned char c : value) {
        if (c == '=') {
            break;
        }

        int val = -1;
        if (c >= 'A' && c <= 'Z') {
            val = c - 'A';
        }
        else if (c >= 'a' && c <= 'z') {
            val = c - 'a';
        }
        else if (c >= '2' && c <= '7') {
            val = c - '2' + 26;
        }
        else {
            continue;
        }

        buffer = (buffer << 5) | static_cast<std::uint32_t>(val);
        bits += 5;

        if (bits >= 8) {
            bits -= 8;
            decoded += static_cast<char>((buffer >> bits) & 0xFF);
        }
    }

    return decoded;
}

