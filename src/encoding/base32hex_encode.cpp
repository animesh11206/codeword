#include <cstdint>
#include <string>
#include "base32hex_encode.h"

std::string base32hex_encode(const std::string& value) {
    static constexpr char alphabet[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUV";

    std::string encoded;
    std::uint32_t buffer = 0;
    unsigned bits = 0;

    for (unsigned char byte : value) {
        buffer = (buffer << 8) | byte;
        bits += 8;

        while (bits >= 5) {
            bits -= 5;
            encoded += alphabet[(buffer >> bits) & 0x1F];
        }
    }

    if (bits > 0) {
        encoded += alphabet[(buffer << (5 - bits)) & 0x1F];
    }

    while (encoded.size() % 8 != 0) {
        encoded += '=';
    }

    return encoded;
}

