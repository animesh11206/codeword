#include <cstdint>
#include <string>
#include "pbkdf2_sha256_hash.h"
#include "hashing_utils.h"

// PBKDF2-HMAC-SHA256 (default salt = "salt", iterations = 1000, output = 32 bytes / 64 hex chars).
std::string pbkdf2_sha256_hash(const std::string& value, const std::string& salt,
                               std::uint32_t iterations, std::size_t dk_len) {
    auto dk = hashing_utils::pbkdf2_raw(hashing_utils::sha256_raw, 64, 32, value, salt, iterations, dk_len);
    return hashing_utils::bytes_to_hex(dk);
}

std::string pbkdf2_sha256_hash(const std::string& value) {
    return pbkdf2_sha256_hash(value, "salt", 1000, 32);
}

