#include <cstdint>
#include <string>
#include "unicode_codepoint_decode.h"
#include "decoding_utils.h"

// Decodes U+XXXX / u+XXXX notation ("U+0048 U+1F600" -> "H😀").
std::string unicode_codepoint_decode(const std::string& value) {
    std::string decoded;
    std::size_t i = 0;

    while (i < value.size()) {
        if ((value[i] == 'U' || value[i] == 'u') && i + 1 < value.size() && value[i + 1] == '+') {
            std::size_t j = i + 2;
            std::uint32_t cp = 0;
            bool has_digits = false;

            while (j < value.size()) {
                int digit = decoding_utils::hex_value(value[j]);
                if (digit < 0) {
                    break;
                }
                cp = (cp << 4) | static_cast<std::uint32_t>(digit);
                has_digits = true;
                ++j;
            }

            if (has_digits) {
                decoding_utils::append_utf8(decoded, cp);
                i = j;
                if (i < value.size() && value[i] == ' ') {
                    ++i;
                }
                continue;
            }
        }

        ++i;
    }

    return decoded;
}

