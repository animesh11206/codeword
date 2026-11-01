#include <cstdint>
#include <string>
#include "utf16_encode.h"
#include "encoding_utils.h"

namespace {

std::string encode_utf16(const std::string& value, bool big_endian, bool with_bom) {
    std::string raw_bytes;

    auto append_unit = [&](std::uint16_t unit) {
        char high = static_cast<char>(unit >> 8);
        char low = static_cast<char>(unit & 0xFF);

        if (big_endian) {
            raw_bytes += high;
            raw_bytes += low;
        }
        else {
            raw_bytes += low;
            raw_bytes += high;
        }
    };

    if (with_bom) {
        append_unit(0xFEFF);
    }

    for (std::uint16_t unit : encoding_utils::to_utf16(value)) {
        append_unit(unit);
    }

    return encoding_utils::bytes_to_hex(raw_bytes);
}

} // namespace

// UTF-16 with a byte order mark, little-endian (hex-rendered wire bytes)
std::string utf16_encode(const std::string& value) {
    return encode_utf16(value, false, true);
}

// UTF-16 big-endian, no byte order mark (hex-rendered wire bytes)
std::string utf16be_encode(const std::string& value) {
    return encode_utf16(value, true, false);
}

// UTF-16 little-endian, no byte order mark (hex-rendered wire bytes)
std::string utf16le_encode(const std::string& value) {
    return encode_utf16(value, false, false);
}
