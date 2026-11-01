#include <string>
#include "shake128_hash.h"
#include "hashing_utils.h"

// FIPS 202 SHAKE128 extensible-output function (default 16 bytes / 32 hex chars).
std::string shake128_hash(const std::string& value, std::size_t out_bytes) {
    return hashing_utils::keccak_sponge_hex(value, 168, 0x1F, out_bytes);
}

std::string shake128_hash(const std::string& value) {
    return shake128_hash(value, 16);
}

