#include <string>
#include "shake256_hash.h"
#include "hashing_utils.h"

// FIPS 202 SHAKE256 extensible-output function (default 32 bytes / 64 hex chars).
std::string shake256_hash(const std::string& value, std::size_t out_bytes) {
    return hashing_utils::keccak_sponge_hex(value, 136, 0x1F, out_bytes);
}

std::string shake256_hash(const std::string& value) {
    return shake256_hash(value, 32);
}

