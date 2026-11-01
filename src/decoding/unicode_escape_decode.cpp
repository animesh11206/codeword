#include <cstdint>
#include <string>
#include <vector>
#include "unicode_escape_decode.h"
#include "decoding_utils.h"

// Decodes \uXXXX (including surrogate pairs) and \UXXXXXXXX escapes back into UTF-8.
std::string unicode_escape_decode(const std::string& value) {
    std::string decoded;
    std::vector<std::uint16_t> units;

    auto flush_units = [&]() {
        if (!units.empty()) {
            decoded += decoding_utils::from_utf16(units);
            units.clear();
        }
    };

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 5 < value.size() && value[i + 1] == 'u') {
            int d0 = decoding_utils::hex_value(value[i + 2]);
            int d1 = decoding_utils::hex_value(value[i + 3]);
            int d2 = decoding_utils::hex_value(value[i + 4]);
            int d3 = decoding_utils::hex_value(value[i + 5]);

            if (d0 >= 0 && d1 >= 0 && d2 >= 0 && d3 >= 0) {
                std::uint16_t unit = static_cast<std::uint16_t>(
                    (d0 << 12) | (d1 << 8) | (d2 << 4) | d3);
                units.push_back(unit);
                i += 5;
                continue;
            }
        }

        if (value[i] == '\\' && i + 9 < value.size() && value[i + 1] == 'U') {
            std::uint32_t cp = 0;
            bool valid = true;
            for (std::size_t k = 2; k <= 9; ++k) {
                int digit = decoding_utils::hex_value(value[i + k]);
                if (digit < 0) {
                    valid = false;
                    break;
                }
                cp = (cp << 4) | static_cast<std::uint32_t>(digit);
            }

            if (valid) {
                flush_units();
                decoding_utils::append_utf8(decoded, cp);
                i += 9;
                continue;
            }
        }

        flush_units();
        decoded += value[i];
    }

    flush_units();
    return decoded;
}
