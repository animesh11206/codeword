#include <cstdint>
#include <string>
#include "utf8_encode.h"
#include "encoding_utils.h"

// Re-encodes the input as clean UTF-8. Invalid byte sequences become U+FFFD.
std::string utf8_encode(const std::string& value) {
    std::string encoded;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        encoding_utils::append_utf8(encoded, code_point);
    }

    return encoded;
}

