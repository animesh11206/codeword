#include <string>
#include "keccak512_hash.h"
#include "hashing_utils.h"

// Original Keccak-512 (0x01 padding) formatted as 128 lowercase hex characters.
std::string keccak512_hash(const std::string& value) {
    return hashing_utils::keccak_sponge_hex(value, 72, 0x01, 64);
}

