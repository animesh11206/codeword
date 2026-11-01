#include <string>
#include "hex_encode.h"

// Each byte as two lowercase hex digits: "Hi" -> "4869"
std::string hex_encode(const std::string& value) {
    static constexpr char digits[] = "0123456789abcdef";

    std::string encoded;
    encoded.reserve(value.size() * 2);

    for (unsigned char byte : value) {
        encoded += digits[byte >> 4];
        encoded += digits[byte & 0x0F];
    }

    return encoded;
}

