#include <string>
#include "sha3_256_hash.h"
#include "hashing_utils.h"

// FIPS 202 SHA3-256 formatted as 64 lowercase hex characters.
std::string sha3_256_hash(const std::string& value) {
    return hashing_utils::keccak_sponge_hex(value, 136, 0x06, 32);
}

