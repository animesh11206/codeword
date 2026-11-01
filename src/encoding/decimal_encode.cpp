#include <cstdint>
#include <string>
#include "decimal_encode.h"
#include "encoding_utils.h"

// Each Unicode code point in decimal, space-separated: "Hé" -> "72 233"
// (Differs from ascii_encode for non-ASCII input, which works per byte.)
std::string decimal_encode(const std::string& value) {
    std::string encoded;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        if (!encoded.empty()) {
            encoded += ' ';
        }
        encoded += std::to_string(code_point);
    }

    return encoded;
}

