#include <algorithm>
#include <string>
#include "uuencode_encode.h"

// Encodes input into uuencoded chunks on a single line (without "begin"/"end" or embedded newlines).
// Each chunk holds up to 45 input bytes, prefixed with its length character.
std::string uuencode_encode(const std::string& value) {
    auto encode_char = [](unsigned bits) {
        return bits == 0 ? '`' : static_cast<char>(bits + 32);
    };

    std::string encoded;

    for (std::size_t i = 0; i < value.size(); i += 45) {
        std::size_t line_length = std::min<std::size_t>(45, value.size() - i);

        encoded += encode_char(static_cast<unsigned>(line_length));

        for (std::size_t j = 0; j < line_length; j += 3) {
            unsigned char b0 = static_cast<unsigned char>(value[i + j]);
            unsigned char b1 = j + 1 < line_length ? static_cast<unsigned char>(value[i + j + 1]) : 0;
            unsigned char b2 = j + 2 < line_length ? static_cast<unsigned char>(value[i + j + 2]) : 0;

            encoded += encode_char(b0 >> 2);
            encoded += encode_char(((b0 & 0x03) << 4) | (b1 >> 4));
            encoded += encode_char(((b1 & 0x0F) << 2) | (b2 >> 6));
            encoded += encode_char(b2 & 0x3F);
        }
    }

    return encoded;
}
