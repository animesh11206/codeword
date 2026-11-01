#include <string>
#include "xor_encode.h"

// XORs every byte with the repeating key. Applying it twice with the same key restores the input.
std::string xor_encode(const std::string& value, const std::string& key) {
    if (key.empty()) {
        return value;
    }

    std::string encoded(value.size(), '\0');

    for (std::size_t i = 0; i < value.size(); ++i) {
        encoded[i] = static_cast<char>(value[i] ^ key[i % key.size()]);
    }

    return encoded;
}

// XORs with the single-byte DEFAULT_XOR_KEY
std::string xor_encode(const std::string& value) {
    return xor_encode(value, std::string(1, static_cast<char>(DEFAULT_XOR_KEY)));
}

