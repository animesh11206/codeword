#include <string>
#include "url_encode.h"

// Percent-encodes every byte except RFC 3986 unreserved characters (A-Z a-z 0-9 - _ . ~)
std::string url_encode(const std::string& value) {
    static constexpr char digits[] = "0123456789ABCDEF";

    std::string encoded;

    for (unsigned char byte : value) {
        bool unreserved = (byte >= 'A' && byte <= 'Z') || (byte >= 'a' && byte <= 'z') ||
                          (byte >= '0' && byte <= '9') ||
                          byte == '-' || byte == '_' || byte == '.' || byte == '~';

        if (unreserved) {
            encoded += static_cast<char>(byte);
        }
        else {
            encoded += '%';
            encoded += digits[byte >> 4];
            encoded += digits[byte & 0x0F];
        }
    }

    return encoded;
}

