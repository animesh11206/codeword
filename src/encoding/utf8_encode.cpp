#include <cstdint>
#include <string>
#include "utf8_encode.h"
#include "encoding_utils.h"

// Normalizes the input as valid UTF-8 (replacing invalid sequences with U+FFFD)
// and emits the UTF-8 wire bytes in lowercase hexadecimal.
std::string utf8_encode(const std::string& value) {
    std::string utf8_bytes;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        encoding_utils::append_utf8(utf8_bytes, code_point);
    }

    return encoding_utils::bytes_to_hex(utf8_bytes);
}
