#include <cstdint>
#include <string>
#include "unicode_iis_encode.h"
#include "encoding_utils.h"

// Each UTF-16 code unit in IIS %uXXXX notation: "Hi" -> "%u0048%u0069"
std::string unicode_iis_encode(const std::string& value) {
    std::string encoded;

    for (std::uint16_t unit : encoding_utils::to_utf16(value)) {
        encoded += "%u";
        encoded += encoding_utils::to_hex(unit, 4, true);
    }

    return encoded;
}

