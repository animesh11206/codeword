#include <cstdint>
#include <string>
#include "utf8_decode.h"
#include "decoding_utils.h"

// Decodes hex-rendered UTF-8 wire bytes and normalizes the UTF-8 byte stream
// (replacing malformed sequences with U+FFFD).
std::string utf8_decode(const std::string& value) {
    static constexpr std::uint32_t min_value[] = {0x0, 0x80, 0x800, 0x10000};

    std::string raw = decoding_utils::hex_to_bytes(value);
    std::string decoded;
    std::size_t i = 0;

    while (i < raw.size()) {
        unsigned char lead = static_cast<unsigned char>(raw[i]);
        std::uint32_t code_point = 0;
        std::size_t extra = 0;

        if (lead < 0x80)                { code_point = lead;        extra = 0; }
        else if ((lead & 0xE0) == 0xC0) { code_point = lead & 0x1F; extra = 1; }
        else if ((lead & 0xF0) == 0xE0) { code_point = lead & 0x0F; extra = 2; }
        else if ((lead & 0xF8) == 0xF0) { code_point = lead & 0x07; extra = 3; }
        else {
            decoding_utils::append_utf8(decoded, 0xFFFD);
            ++i;
            continue;
        }

        if (i + extra >= raw.size()) {
            decoding_utils::append_utf8(decoded, 0xFFFD);
            ++i;
            continue;
        }

        bool valid = true;
        for (std::size_t j = 1; j <= extra; ++j) {
            unsigned char next = static_cast<unsigned char>(raw[i + j]);
            if ((next & 0xC0) != 0x80) {
                valid = false;
                break;
            }
            code_point = (code_point << 6) | (next & 0x3F);
        }

        if (!valid || code_point < min_value[extra] || code_point > 0x10FFFF ||
            (code_point >= 0xD800 && code_point <= 0xDFFF)) {
            decoding_utils::append_utf8(decoded, 0xFFFD);
            ++i;
            continue;
        }

        decoding_utils::append_utf8(decoded, code_point);
        i += extra + 1;
    }

    return decoded;
}
