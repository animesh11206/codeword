#include <cstdint>
#include <string>
#include "html_entity_hex_encode.h"
#include "encoding_utils.h"

// Each code point as a hex HTML entity: "Hi" -> "&#x48;&#x69;"
std::string html_entity_hex_encode(const std::string& value) {
    std::string encoded;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        encoded += "&#x";
        encoded += encoding_utils::to_hex(code_point, 1, true);
        encoded += ';';
    }

    return encoded;
}

