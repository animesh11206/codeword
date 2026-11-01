#include <string>
#include "sha3_512_hash.h"
#include "hashing_utils.h"

// FIPS 202 SHA3-512 formatted as 128 lowercase hex characters.
std::string sha3_512_hash(const std::string& value) {
    return hashing_utils::keccak_sponge_hex(value, 72, 0x06, 64);
}

