#include <cstdint>
#include <string>
#include "unicode_codepoint_encode.h"
#include "encoding_utils.h"

// Each code point in U+XXXX notation, space-separated: "Hi" -> "U+0048 U+0069"
std::string unicode_codepoint_encode(const std::string& value) {
    std::string encoded;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        if (!encoded.empty()) {
            encoded += ' ';
        }
        encoded += "U+";
        encoded += encoding_utils::to_hex(code_point, 4, true);
    }

    return encoded;
}

