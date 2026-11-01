#include <cstdint>
#include <string>
#include "html_entity_dec_encode.h"
#include "encoding_utils.h"

// Each code point as a decimal HTML entity: "Hi" -> "&#72;&#105;"
std::string html_entity_dec_encode(const std::string& value) {
    std::string encoded;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        encoded += "&#";
        encoded += std::to_string(code_point);
        encoded += ';';
    }

    return encoded;
}

