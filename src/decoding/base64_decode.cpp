#include <cstdint>
#include <string>
#include "base64_decode.h"

std::string base64_decode(const std::string& value) {
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
            val = c - 'a' + 26;
        }
        else if (c >= '0' && c <= '9') {
            val = c - '0' + 52;
        }
        else if (c == '+') {
            val = 62;
        }
        else if (c == '/') {
            val = 63;
        }
        else {
            continue;
        }

        buffer = (buffer << 6) | static_cast<std::uint32_t>(val);
        bits += 6;

        if (bits >= 8) {
            bits -= 8;
            decoded += static_cast<char>((buffer >> bits) & 0xFF);
        }
    }

    return decoded;
}

