#include <string>
#include "sha3_384_hash.h"
#include "hashing_utils.h"

// FIPS 202 SHA3-384 formatted as 96 lowercase hex characters.
std::string sha3_384_hash(const std::string& value) {
    return hashing_utils::keccak_sponge_hex(value, 104, 0x06, 48);
}

