#include <string>
#include "quoted_printable_encode.h"
#include "encoding_utils.h"

// RFC 2045 quoted-printable. Lines are soft-wrapped with "=\r\n" to stay within 76 characters.
std::string quoted_printable_encode(const std::string& value) {
    std::string encoded;
    std::size_t line_length = 0;

    auto append = [&](const std::string& token) {
        // Leave room for the trailing '=' of a soft line break
        if (line_length + token.size() > 75) {
            encoded += "=\r\n";
            line_length = 0;
        }
        encoded += token;
        line_length += token.size();
    };

    for (std::size_t i = 0; i < value.size(); ++i) {
        unsigned char byte = static_cast<unsigned char>(value[i]);
        bool is_last = (i + 1 == value.size());

        // Space/tab are literal unless they end the data (trailing whitespace must be encoded)
        bool literal = (byte >= 33 && byte <= 126 && byte != '=') ||
                       ((byte == ' ' || byte == '\t') && !is_last);

        if (literal) {
            append(std::string(1, static_cast<char>(byte)));
        }
        else {
            append("=" + encoding_utils::to_hex(byte, 2, true));
        }
    }

    return encoded;
}

