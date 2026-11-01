#ifndef ENCODING_UTILS_H
#define ENCODING_UTILS_H

#include <cstdint>
#include <string>
#include <vector>

namespace encoding_utils {

// Decodes a UTF-8 string into Unicode code points.
// Invalid or truncated sequences are replaced with U+FFFD.
inline std::vector<std::uint32_t> decode_utf8(const std::string& value) {
    static constexpr std::uint32_t min_value[] = {0x0, 0x80, 0x800, 0x10000};

    std::vector<std::uint32_t> code_points;
    std::size_t i = 0;

    while (i < value.size()) {
        unsigned char lead = static_cast<unsigned char>(value[i]);
        std::uint32_t code_point = 0;
        std::size_t extra = 0;

        if (lead < 0x80)                { code_point = lead;        extra = 0; }
        else if ((lead & 0xE0) == 0xC0) { code_point = lead & 0x1F; extra = 1; }
        else if ((lead & 0xF0) == 0xE0) { code_point = lead & 0x0F; extra = 2; }
        else if ((lead & 0xF8) == 0xF0) { code_point = lead & 0x07; extra = 3; }
        else {
            code_points.push_back(0xFFFD);
            ++i;
            continue;
        }

        // Truncated sequence: not enough continuation bytes left
        if (i + extra >= value.size()) {
            code_points.push_back(0xFFFD);
            ++i;
            continue;
        }

        bool valid = true;
        for (std::size_t j = 1; j <= extra; ++j) {
            unsigned char next = static_cast<unsigned char>(value[i + j]);
            if ((next & 0xC0) != 0x80) {
                valid = false;
                break;
            }
            code_point = (code_point << 6) | (next & 0x3F);
        }

        if (!valid || code_point < min_value[extra] || code_point > 0x10FFFF ||
            (code_point >= 0xD800 && code_point <= 0xDFFF)) {
            code_points.push_back(0xFFFD);
            ++i;
            continue;
        }

        code_points.push_back(code_point);
        i += extra + 1;
    }

    return code_points;
}

// Appends a code point to a string as UTF-8.
inline void append_utf8(std::string& output, std::uint32_t code_point) {
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

// Converts a UTF-8 string into UTF-16 code units (with surrogate pairs).
inline std::vector<std::uint16_t> to_utf16(const std::string& value) {
    std::vector<std::uint16_t> units;

    for (std::uint32_t code_point : decode_utf8(value)) {
        if (code_point >= 0x10000) {
            code_point -= 0x10000;
            units.push_back(static_cast<std::uint16_t>(0xD800 + (code_point >> 10)));
            units.push_back(static_cast<std::uint16_t>(0xDC00 + (code_point & 0x3FF)));
        }
        else {
            units.push_back(static_cast<std::uint16_t>(code_point));
        }
    }

    return units;
}

// Formats a number as hex, padded with zeros to at least min_width digits.
inline std::string to_hex(std::uint32_t value, std::size_t min_width, bool uppercase = true) {
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string hex;

    do {
        hex.insert(hex.begin(), digits[value & 0xF]);
        value >>= 4;
    } while (value != 0);

    while (hex.size() < min_width) {
        hex.insert(hex.begin(), '0');
    }

    return hex;
}

// Formats a raw byte string as lowercase hexadecimal (2 hex chars per byte).
inline std::string bytes_to_hex(const std::string& bytes, bool uppercase = false) {
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string hex;
    hex.reserve(bytes.size() * 2);

    for (unsigned char b : bytes) {
        hex += digits[(b >> 4) & 0x0F];
        hex += digits[b & 0x0F];
    }

    return hex;
}

// Treats the input bytes as one big-endian number and converts it to the given base.
// Leading zero bytes are preserved as leading alphabet[0] characters (base58/base62 style).
inline std::string encode_big_base(const std::string& value, const char* alphabet, unsigned base) {
    std::size_t leading_zeros = 0;
    while (leading_zeros < value.size() && value[leading_zeros] == '\0') {
        ++leading_zeros;
    }

    std::vector<unsigned char> digits; // little-endian digits in the target base

    for (std::size_t i = leading_zeros; i < value.size(); ++i) {
        unsigned carry = static_cast<unsigned char>(value[i]);

        for (unsigned char& digit : digits) {
            carry += static_cast<unsigned>(digit) << 8;
            digit = static_cast<unsigned char>(carry % base);
            carry /= base;
        }

        while (carry > 0) {
            digits.push_back(static_cast<unsigned char>(carry % base));
            carry /= base;
        }
    }

    std::string encoded(leading_zeros, alphabet[0]);
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        encoded += alphabet[*it];
    }

    return encoded;
}

} // namespace encoding_utils

#endif
