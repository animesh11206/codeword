#include <string>
#include "xor_decode.h"

// XOR is symmetric: XORing with the same repeating key restores the original plaintext.
std::string xor_decode(const std::string& value, const std::string& key) {
    if (key.empty()) {
        return value;
    }

    std::string decoded(value.size(), '\0');

    for (std::size_t i = 0; i < value.size(); ++i) {
        decoded[i] = static_cast<char>(value[i] ^ key[i % key.size()]);
    }

    return decoded;
}

// XOR-decodes with the single-byte DEFAULT_XOR_KEY (0x5A).
std::string xor_decode(const std::string& value) {
    return xor_decode(value, std::string(1, static_cast<char>(DEFAULT_XOR_KEY)));
}

