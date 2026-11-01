#include <string>
#include "keccak256_hash.h"
#include "hashing_utils.h"

// Original Keccak-256 (0x01 padding, Ethereum style) formatted as 64 lowercase hex characters.
std::string keccak256_hash(const std::string& value) {
    return hashing_utils::keccak_sponge_hex(value, 136, 0x01, 32);
}

