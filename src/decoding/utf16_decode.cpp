#include <cstdint>
#include <string>
#include <vector>
#include "utf16_decode.h"
#include "decoding_utils.h"

namespace {

std::string decode_utf16_bytes(const std::string& value, bool big_endian, std::size_t start_offset) {
    std::vector<std::uint16_t> units;

    for (std::size_t i = start_offset; i + 1 < value.size(); i += 2) {
        unsigned char b0 = static_cast<unsigned char>(value[i]);
        unsigned char b1 = static_cast<unsigned char>(value[i + 1]);

        std::uint16_t unit = big_endian
            ? static_cast<std::uint16_t>((b0 << 8) | b1)
            : static_cast<std::uint16_t>((b1 << 8) | b0);

        units.push_back(unit);
    }

    return decoding_utils::from_utf16(units);
}

} // namespace

// Decodes UTF-16 bytes into UTF-8, inspecting the BOM if present (defaults to little-endian).
std::string utf16_decode(const std::string& value) {
    if (value.size() >= 2) {
        unsigned char b0 = static_cast<unsigned char>(value[0]);
        unsigned char b1 = static_cast<unsigned char>(value[1]);

        if (b0 == 0xFE && b1 == 0xFF) {
            return decode_utf16_bytes(value, true, 2);
        }
        if (b0 == 0xFF && b1 == 0xFE) {
            return decode_utf16_bytes(value, false, 2);
        }
    }

    return decode_utf16_bytes(value, false, 0);
}

// Decodes big-endian UTF-16 bytes into UTF-8.
std::string utf16be_decode(const std::string& value) {
    return decode_utf16_bytes(value, true, 0);
}

// Decodes little-endian UTF-16 bytes into UTF-8.
std::string utf16le_decode(const std::string& value) {
    return decode_utf16_bytes(value, false, 0);
}

