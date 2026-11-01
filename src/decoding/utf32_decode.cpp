#include <cstdint>
#include <string>
#include "utf32_decode.h"
#include "decoding_utils.h"

namespace {

std::string decode_utf32_bytes(const std::string& value, bool big_endian, std::size_t start_offset) {
    std::string decoded;

    for (std::size_t i = start_offset; i + 3 < value.size(); i += 4) {
        unsigned char b0 = static_cast<unsigned char>(value[i]);
        unsigned char b1 = static_cast<unsigned char>(value[i + 1]);
        unsigned char b2 = static_cast<unsigned char>(value[i + 2]);
        unsigned char b3 = static_cast<unsigned char>(value[i + 3]);

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

// Decodes UTF-32 bytes into UTF-8, inspecting the BOM if present (defaults to little-endian).
std::string utf32_decode(const std::string& value) {
    if (value.size() >= 4) {
        unsigned char b0 = static_cast<unsigned char>(value[0]);
        unsigned char b1 = static_cast<unsigned char>(value[1]);
        unsigned char b2 = static_cast<unsigned char>(value[2]);
        unsigned char b3 = static_cast<unsigned char>(value[3]);

        if (b0 == 0x00 && b1 == 0x00 && b2 == 0xFE && b3 == 0xFF) {
            return decode_utf32_bytes(value, true, 4);
        }
        if (b0 == 0xFF && b1 == 0xFE && b2 == 0x00 && b3 == 0x00) {
            return decode_utf32_bytes(value, false, 4);
        }
    }

    return decode_utf32_bytes(value, false, 0);
}

// Decodes big-endian UTF-32 bytes into UTF-8.
std::string utf32be_decode(const std::string& value) {
    return decode_utf32_bytes(value, true, 0);
}

// Decodes little-endian UTF-32 bytes into UTF-8.
std::string utf32le_decode(const std::string& value) {
    return decode_utf32_bytes(value, false, 0);
}

