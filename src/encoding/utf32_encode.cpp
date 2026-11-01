#include <cstdint>
#include <string>
#include "utf32_encode.h"
#include "encoding_utils.h"

namespace {

std::string encode_utf32(const std::string& value, bool big_endian, bool with_bom) {
    std::string raw_bytes;

    auto append_unit = [&](std::uint32_t unit) {
        char bytes[4] = {
            static_cast<char>((unit >> 24) & 0xFF),
            static_cast<char>((unit >> 16) & 0xFF),
            static_cast<char>((unit >> 8) & 0xFF),
            static_cast<char>(unit & 0xFF)
        };

        if (big_endian) {
            raw_bytes.append(bytes, 4);
        }
        else {
            for (int i = 3; i >= 0; --i) {
                raw_bytes += bytes[i];
            }
        }
    };

    if (with_bom) {
        append_unit(0xFEFF);
    }

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        append_unit(code_point);
    }

    return encoding_utils::bytes_to_hex(raw_bytes);
}

} // namespace

// UTF-32 with a byte order mark, little-endian (hex-rendered wire bytes)
std::string utf32_encode(const std::string& value) {
    return encode_utf32(value, false, true);
}

// UTF-32 big-endian, no byte order mark (hex-rendered wire bytes)
std::string utf32be_encode(const std::string& value) {
    return encode_utf32(value, true, false);
}

// UTF-32 little-endian, no byte order mark (hex-rendered wire bytes)
std::string utf32le_encode(const std::string& value) {
    return encode_utf32(value, false, false);
}
