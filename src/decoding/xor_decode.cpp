#include <string>
#include "xor_decode.h"
#include "decoding_utils.h"

// Decodes hex-rendered XOR bytes and XORs with the repeating key to restore plaintext.
std::string xor_decode(const std::string& value, const std::string& key) {
    std::string raw = decoding_utils::hex_to_bytes(value);
    if (key.empty()) {
        return raw;
    }

    std::string decoded(raw.size(), '\0');

    for (std::size_t i = 0; i < raw.size(); ++i) {
        decoded[i] = static_cast<char>(raw[i] ^ key[i % key.size()]);
    }

    return decoded;
}

// XOR-decodes hex-rendered input with the single-byte DEFAULT_XOR_KEY (0x5A).
std::string xor_decode(const std::string& value) {
    return xor_decode(value, std::string(1, static_cast<char>(DEFAULT_XOR_KEY)));
}
