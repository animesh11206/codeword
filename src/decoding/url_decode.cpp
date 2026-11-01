#include <string>
#include "url_decode.h"
#include "decoding_utils.h"

// Decodes percent-encoded URL strings ("a%20b%2F%C3%A9" -> "a b/é").
std::string url_decode(const std::string& value) {
    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            int high = decoding_utils::hex_value(value[i + 1]);
            int low = decoding_utils::hex_value(value[i + 2]);

            if (high >= 0 && low >= 0) {
                decoded += static_cast<char>((high << 4) | low);
                i += 2;
                continue;
            }
        }
        else if (value[i] == '+') {
            decoded += ' ';
            continue;
        }

        decoded += value[i];
    }

    return decoded;
}

