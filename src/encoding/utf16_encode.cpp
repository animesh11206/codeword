#include <cstdint>
#include <string>
#include "utf16_encode.h"
#include "encoding_utils.h"

namespace {

std::string encode_utf16(const std::string& value, bool big_endian, bool with_bom) {
    std::string encoded;

    auto append_unit = [&](std::uint16_t unit) {
        char high = static_cast<char>(unit >> 8);
        char low = static_cast<char>(unit & 0xFF);

        if (big_endian) {
            encoded += high;
            encoded += low;
        }
        else {
            encoded += low;
            encoded += high;
        }
    };

    if (with_bom) {
        append_unit(0xFEFF);
    }

    for (std::uint16_t unit : encoding_utils::to_utf16(value)) {
        append_unit(unit);
    }

    return encoded;
}

} // namespace

// UTF-16 with a byte order mark, little-endian (same as Python's "utf-16" on Windows/x86)
std::string utf16_encode(const std::string& value) {
    return encode_utf16(value, false, true);
}

// UTF-16 big-endian, no byte order mark
std::string utf16be_encode(const std::string& value) {
    return encode_utf16(value, true, false);
}

// UTF-16 little-endian, no byte order mark
std::string utf16le_encode(const std::string& value) {
    return encode_utf16(value, false, false);
}

