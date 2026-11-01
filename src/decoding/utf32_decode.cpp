#include <cstdint>
#include <string>
#include "utf32_decode.h"
#include "decoding_utils.h"

namespace {

std::string decode_utf32_bytes(const std::string& raw, bool big_endian, std::size_t start_offset) {
    std::string decoded;

    for (std::size_t i = start_offset; i + 3 < raw.size(); i += 4) {
        unsigned char b0 = static_cast<unsigned char>(raw[i]);
        unsigned char b1 = static_cast<unsigned char>(raw[i + 1]);
        unsigned char b2 = static_cast<unsigned char>(raw[i + 2]);
        unsigned char b3 = static_cast<unsigned char>(raw[i + 3]);

        std::uint32_t cp = big_endian
            ? ((static_cast<std::uint32_t>(b0) << 24) |
               (static_cast<std::uint32_t>(b1) << 16) |
               (static_cast<std::uint32_t>(b2) << 8)  |
                static_cast<std::uint32_t>(b3))
            : ((static_cast<std::uint32_t>(b3) << 24) |
               (static_cast<std::uint32_t>(b2) << 16) |
               (static_cast<std::uint32_t>(b1) << 8)  |
                static_cast<std::uint32_t>(b0));

        decoding_utils::append_utf8(decoded, cp);
    }

    return decoded;
}

} // namespace

// Decodes hex-rendered UTF-32 wire bytes into UTF-8, inspecting the BOM if present (defaults to LE).
std::string utf32_decode(const std::string& value) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    if (raw.size() >= 4) {
        unsigned char b0 = static_cast<unsigned char>(raw[0]);
        unsigned char b1 = static_cast<unsigned char>(raw[1]);
        unsigned char b2 = static_cast<unsigned char>(raw[2]);
        unsigned char b3 = static_cast<unsigned char>(raw[3]);

        if (b0 == 0x00 && b1 == 0x00 && b2 == 0xFE && b3 == 0xFF) {
            return decode_utf32_bytes(raw, true, 4);
        }
        if (b0 == 0xFF && b1 == 0xFE && b2 == 0x00 && b3 == 0x00) {
            return decode_utf32_bytes(raw, false, 4);
        }
    }

    return decode_utf32_bytes(raw, false, 0);
}

// Decodes hex-rendered big-endian UTF-32 wire bytes into UTF-8.
std::string utf32be_decode(const std::string& value) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    return decode_utf32_bytes(raw, true, 0);
}

// Decodes hex-rendered little-endian UTF-32 wire bytes into UTF-8.
std::string utf32le_decode(const std::string& value) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    return decode_utf32_bytes(raw, false, 0);
}
