#include <string>
#include "sha3_224_hash.h"
#include "hashing_utils.h"

// FIPS 202 SHA3-224 formatted as 56 lowercase hex characters.
std::string sha3_224_hash(const std::string& value) {
    return hashing_utils::keccak_sponge_hex(value, 144, 0x06, 28);
}

