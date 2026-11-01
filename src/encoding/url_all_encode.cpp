#include <string>
#include "url_all_encode.h"

// Percent-encodes every byte, including letters and digits: "Hi" -> "%48%69"
std::string url_all_encode(const std::string& value) {
    static constexpr char digits[] = "0123456789ABCDEF";

    std::string encoded;
    encoded.reserve(value.size() * 3);

    for (unsigned char byte : value) {
        encoded += '%';
        encoded += digits[byte >> 4];
        encoded += digits[byte & 0x0F];
    }

    return encoded;
}

