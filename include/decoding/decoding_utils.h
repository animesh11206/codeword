#ifndef DECODING_UTILS_H
#define DECODING_UTILS_H

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace decoding_utils {

// Appends a Unicode code point to a string as UTF-8.
inline void append_utf8(std::string& output, std::uint32_t code_point) {
    if (code_point > 0x10FFFF || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        code_point = 0xFFFD;
    }

    if (code_point < 0x80) {
        output += static_cast<char>(code_point);
    }
    else if (code_point < 0x800) {
        output += static_cast<char>(0xC0 | (code_point >> 6));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    }
    else if (code_point < 0x10000) {
        output += static_cast<char>(0xE0 | (code_point >> 12));
        output += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    }
    else {
        output += static_cast<char>(0xF0 | (code_point >> 18));
        output += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
        output += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    }
}

// Converts a sequence of UTF-16 code units (including surrogate pairs) into UTF-8.
inline std::string from_utf16(const std::vector<std::uint16_t>& units) {
    std::string output;

    for (std::size_t i = 0; i < units.size(); ++i) {
        std::uint16_t u = units[i];

        if (u >= 0xD800 && u <= 0xDBFF) {
            if (i + 1 < units.size() && units[i + 1] >= 0xDC00 && units[i + 1] <= 0xDFFF) {
                std::uint32_t cp = 0x10000 + ((static_cast<std::uint32_t>(u - 0xD800) << 10) |
                                              static_cast<std::uint32_t>(units[i + 1] - 0xDC00));
                append_utf8(output, cp);
                ++i;
            }
            else {
                append_utf8(output, 0xFFFD);
            }
        }
        else if (u >= 0xDC00 && u <= 0xDFFF) {
            append_utf8(output, 0xFFFD);
        }
        else {
            append_utf8(output, u);
        }
    }

    return output;
}

// Returns 0..15 for a valid hex digit, or -1 otherwise.
inline int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Decodes a big-integer base-N string (Base58 / Base62) back into raw bytes.
inline std::string decode_big_base(const std::string& value, const char* alphabet, unsigned base) {
    std::size_t leading_zeros = 0;
    while (leading_zeros < value.size() && value[leading_zeros] == alphabet[0]) {
        ++leading_zeros;
    }

    std::vector<unsigned char> bytes; // little-endian base-256 bytes

    for (std::size_t i = leading_zeros; i < value.size(); ++i) {
        const char* pos = std::strchr(alphabet, value[i]);
        if (pos == nullptr) {
            continue;
        }

        unsigned carry = static_cast<unsigned>(pos - alphabet);

        for (unsigned char& byte : bytes) {
            carry += static_cast<unsigned>(byte) * base;
            byte = static_cast<unsigned char>(carry & 0xFF);
            carry >>= 8;
        }

        while (carry > 0) {
            bytes.push_back(static_cast<unsigned char>(carry & 0xFF));
            carry >>= 8;
        }
    }

    std::string decoded(leading_zeros, '\0');
    for (auto it = bytes.rbegin(); it != bytes.rend(); ++it) {
        decoded += static_cast<char>(*it);
    }

    return decoded;
}

} // namespace decoding_utils

#endif

