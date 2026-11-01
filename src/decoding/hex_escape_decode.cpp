#include <string>
#include "hex_escape_decode.h"
#include "decoding_utils.h"

// Decodes \xNN hex escapes ("\x48\x69" -> "Hi").
std::string hex_escape_decode(const std::string& value) {
    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 3 < value.size() && (value[i + 1] == 'x' || value[i + 1] == 'X')) {
            int high = decoding_utils::hex_value(value[i + 2]);
            int low = decoding_utils::hex_value(value[i + 3]);

            if (high >= 0 && low >= 0) {
                decoded += static_cast<char>((high << 4) | low);
                i += 3;
                continue;
            }
        }

        decoded += value[i];
    }

    return decoded;
}

