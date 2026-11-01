#include <cstdint>
#include <string>
#include "html_entity_dec_decode.h"
#include "decoding_utils.h"

// Decodes decimal HTML entities ("&#72;&#105;" -> "Hi").
std::string html_entity_dec_decode(const std::string& value) {
    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '&' && i + 2 < value.size() && value[i + 1] == '#') {
            std::size_t j = i + 2;
            std::uint32_t code_point = 0;
            bool has_digits = false;

            while (j < value.size() && value[j] >= '0' && value[j] <= '9') {
                code_point = code_point * 10 + static_cast<std::uint32_t>(value[j] - '0');
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

