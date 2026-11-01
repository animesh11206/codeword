#include <cstdint>
#include <string>
#include "html_entity_hex_decode.h"
#include "decoding_utils.h"

// Decodes hexadecimal HTML entities ("&#x48;&#x69;" -> "Hi").
std::string html_entity_hex_decode(const std::string& value) {
    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '&' && i + 3 < value.size() && value[i + 1] == '#' &&
            (value[i + 2] == 'x' || value[i + 2] == 'X')) {
            std::size_t j = i + 3;
            std::uint32_t code_point = 0;
            bool has_digits = false;

            while (j < value.size()) {
                int digit = decoding_utils::hex_value(value[j]);
                if (digit < 0) {
                    break;
                }
                code_point = (code_point << 4) | static_cast<std::uint32_t>(digit);
                has_digits = true;
                ++j;
            }

            if (has_digits && j < value.size() && value[j] == ';') {
                decoding_utils::append_utf8(decoded, code_point);
                i = j;
                continue;
            }
        }

        decoded += value[i];
    }

    return decoded;
}

