#include <cstdint>
#include <string>
#include "unicode_escape_encode.h"
#include "encoding_utils.h"

// Each UTF-16 code unit as \uXXXX (JavaScript/Java/JSON style).
// Characters above U+FFFF become surrogate pairs: U+1F600 -> "\uD83D\uDE00"
std::string unicode_escape_encode(const std::string& value) {
    std::string encoded;

    for (std::uint16_t unit : encoding_utils::to_utf16(value)) {
        encoded += "\\u";
        encoded += encoding_utils::to_hex(unit, 4, true);
    }

    return encoded;
}

