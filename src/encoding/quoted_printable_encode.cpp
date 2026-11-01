#include <string>
#include "quoted_printable_encode.h"
#include "encoding_utils.h"

// RFC 2045 quoted-printable (single-line mode for line-by-line wordlist processing).
std::string quoted_printable_encode(const std::string& value) {
    std::string encoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        unsigned char byte = static_cast<unsigned char>(value[i]);
        bool is_last = (i + 1 == value.size());

        // Space/tab are literal unless they end the data (trailing whitespace must be encoded)
        bool literal = (byte >= 33 && byte <= 126 && byte != '=') ||
                       ((byte == ' ' || byte == '\t') && !is_last);

        if (literal) {
            encoded += static_cast<char>(byte);
        }
        else {
            encoded += "=" + encoding_utils::to_hex(byte, 2, true);
        }
    }

    return encoded;
}
