#include <cstdint>
#include <string>
#include <vector>
#include "utf16_decode.h"
#include "decoding_utils.h"

namespace {

std::string decode_utf16_bytes(const std::string& raw, bool big_endian, std::size_t start_offset) {
    std::vector<std::uint16_t> units;

    for (std::size_t i = start_offset; i + 1 < raw.size(); i += 2) {
        unsigned char b0 = static_cast<unsigned char>(raw[i]);
        unsigned char b1 = static_cast<unsigned char>(raw[i + 1]);

        std::uint16_t unit = big_endian
            ? static_cast<std::uint16_t>((b0 << 8) | b1)
            : static_cast<std::uint16_t>((b1 << 8) | b0);

        units.push_back(unit);
    }

    return decoding_utils::from_utf16(units);
}

} // namespace

// Decodes hex-rendered UTF-16 wire bytes into UTF-8, inspecting the BOM if present (defaults to LE).
std::string utf16_decode(const std::string& value) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    if (raw.size() >= 2) {
        unsigned char b0 = static_cast<unsigned char>(raw[0]);
        unsigned char b1 = static_cast<unsigned char>(raw[1]);

        if (b0 == 0xFE && b1 == 0xFF) {
            return decode_utf16_bytes(raw, true, 2);
        }
        if (b0 == 0xFF && b1 == 0xFE) {
            return decode_utf16_bytes(raw, false, 2);
        }
    }

    return decode_utf16_bytes(raw, false, 0);
}

// Decodes hex-rendered big-endian UTF-16 wire bytes into UTF-8.
std::string utf16be_decode(const std::string& value) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    return decode_utf16_bytes(raw, true, 0);
}

// Decodes hex-rendered little-endian UTF-16 wire bytes into UTF-8.
std::string utf16le_decode(const std::string& value) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    return decode_utf16_bytes(raw, false, 0);
}
