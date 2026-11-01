#include <string>
#include "hex_escape_encode.h"

// Each byte as a \xNN escape: "Hi" -> "\x48\x69"
std::string hex_escape_encode(const std::string& value) {
    static constexpr char digits[] = "0123456789abcdef";

    std::string encoded;
    encoded.reserve(value.size() * 4);

    for (unsigned char byte : value) {
        encoded += "\\x";
        encoded += digits[byte >> 4];
        encoded += digits[byte & 0x0F];
    }

    return encoded;
}

