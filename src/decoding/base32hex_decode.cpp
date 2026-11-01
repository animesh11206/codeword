#include <cstdint>
#include <string>
#include "base32hex_decode.h"

std::string base32hex_decode(const std::string& value) {
    std::string decoded;
    std::uint32_t buffer = 0;
    unsigned bits = 0;

    for (unsigned char c : value) {
        if (c == '=') {
            break;
        }

        int val = -1;
        if (c >= '0' && c <= '9') {
            val = c - '0';
        }
        else if (c >= 'A' && c <= 'V') {
            val = c - 'A' + 10;
        }
        else if (c >= 'a' && c <= 'v') {
            val = c - 'a' + 10;
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

