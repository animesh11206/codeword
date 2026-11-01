#include <string>
#include "quoted_printable_decode.h"
#include "decoding_utils.h"

// Decodes RFC 2045 quoted-printable strings (including =XX hex escapes and soft line breaks).
std::string quoted_printable_decode(const std::string& value) {
    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '=') {
            // Soft line break: =\r\n or =\n
            if (i + 1 < value.size() && value[i + 1] == '\r') {
                if (i + 2 < value.size() && value[i + 2] == '\n') {
                    i += 2;
                }
                else {
                    ++i;
                }
                continue;
            }
            if (i + 1 < value.size() && value[i + 1] == '\n') {
                ++i;
                continue;
            }

            if (i + 2 < value.size()) {
                int high = decoding_utils::hex_value(value[i + 1]);
                int low = decoding_utils::hex_value(value[i + 2]);

                if (high >= 0 && low >= 0) {
                    decoded += static_cast<char>((high << 4) | low);
                    i += 2;
                    continue;
                }
            }
        }

        decoded += value[i];
    }

    return decoded;
}

