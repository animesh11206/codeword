#include <cstdint>
#include <string>
#include "decimal_decode.h"
#include "decoding_utils.h"

// Decodes space-separated decimal Unicode code points ("72 233" -> "Hé").
std::string decimal_decode(const std::string& value) {
    std::string decoded;
    std::uint32_t current = 0;
    bool in_number = false;

    for (unsigned char c : value) {
        if (c >= '0' && c <= '9') {
            current = current * 10 + (c - '0');
            in_number = true;
        }
        else if (in_number) {
            decoding_utils::append_utf8(decoded, current);
            current = 0;
            in_number = false;
        }
    }

    if (in_number) {
        decoding_utils::append_utf8(decoded, current);
    }

    return decoded;
}

