#include <string>
#include "xor_encode.h"
#include "encoding_utils.h"

// XORs every byte with the repeating key and returns lowercase hex wire bytes.
std::string xor_encode(const std::string& value, const std::string& key) {
    if (key.empty()) {
        return encoding_utils::bytes_to_hex(value);
    }

    std::string xored(value.size(), '\0');

    for (std::size_t i = 0; i < value.size(); ++i) {
        xored[i] = static_cast<char>(value[i] ^ key[i % key.size()]);
    }

    return encoding_utils::bytes_to_hex(xored);
}

// XORs with the single-byte DEFAULT_XOR_KEY (0x5A) and returns lowercase hex.
std::string xor_encode(const std::string& value) {
    return xor_encode(value, std::string(1, static_cast<char>(DEFAULT_XOR_KEY)));
}
