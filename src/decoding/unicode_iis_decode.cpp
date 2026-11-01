#include <cstdint>
#include <string>
#include <vector>
#include "unicode_iis_decode.h"
#include "decoding_utils.h"

// Decodes IIS %uXXXX escapes (including surrogate pairs) back into UTF-8.
std::string unicode_iis_decode(const std::string& value) {
    std::string decoded;
    std::vector<std::uint16_t> units;

    auto flush_units = [&]() {
        if (!units.empty()) {
            decoded += decoding_utils::from_utf16(units);
            units.clear();
        }
    };

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 5 < value.size() && (value[i + 1] == 'u' || value[i + 1] == 'U')) {
            int d0 = decoding_utils::hex_value(value[i + 2]);
            int d1 = decoding_utils::hex_value(value[i + 3]);
            int d2 = decoding_utils::hex_value(value[i + 4]);
            int d3 = decoding_utils::hex_value(value[i + 5]);

            if (d0 >= 0 && d1 >= 0 && d2 >= 0 && d3 >= 0) {
                std::uint16_t unit = static_cast<std::uint16_t>((d0 << 12) | (d1 << 8) | (d2 << 4) | d3);
                units.push_back(unit);
                i += 5;
                continue;
            }
        }

        flush_units();
        decoded += value[i];
    }

    flush_units();
    return decoded;
}

