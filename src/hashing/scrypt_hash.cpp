#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "scrypt_hash.h"
#include "hashing_utils.h"

namespace {

void salsa20_8(unsigned char b[64]) {
    std::uint32_t b32[16];
    std::uint32_t x[16];

    for (int i = 0; i < 16; ++i) {
        b32[i] = hashing_utils::load_le32(b + i * 4);
        x[i] = b32[i];
    }

    for (int i = 0; i < 8; i += 2) {
        x[ 4] ^= hashing_utils::rotl32(x[ 0] + x[12], 7);
        x[ 8] ^= hashing_utils::rotl32(x[ 4] + x[ 0], 9);
        x[12] ^= hashing_utils::rotl32(x[ 8] + x[ 4], 13);
        x[ 0] ^= hashing_utils::rotl32(x[12] + x[ 8], 18);

        x[ 9] ^= hashing_utils::rotl32(x[ 5] + x[ 1], 7);
        x[13] ^= hashing_utils::rotl32(x[ 9] + x[ 5], 9);
        x[ 1] ^= hashing_utils::rotl32(x[13] + x[ 9], 13);
        x[ 5] ^= hashing_utils::rotl32(x[ 1] + x[13], 18);

        x[14] ^= hashing_utils::rotl32(x[10] + x[ 6], 7);
        x[ 2] ^= hashing_utils::rotl32(x[14] + x[10], 9);
        x[ 6] ^= hashing_utils::rotl32(x[ 2] + x[14], 13);
        x[10] ^= hashing_utils::rotl32(x[ 6] + x[ 2], 18);

        x[ 3] ^= hashing_utils::rotl32(x[15] + x[11], 7);
        x[ 7] ^= hashing_utils::rotl32(x[ 3] + x[15], 9);
        x[11] ^= hashing_utils::rotl32(x[ 7] + x[ 3], 13);
        x[15] ^= hashing_utils::rotl32(x[11] + x[ 7], 18);

        x[ 1] ^= hashing_utils::rotl32(x[ 0] + x[ 3], 7);
        x[ 2] ^= hashing_utils::rotl32(x[ 1] + x[ 0], 9);
        x[ 3] ^= hashing_utils::rotl32(x[ 2] + x[ 1], 13);
        x[ 0] ^= hashing_utils::rotl32(x[ 3] + x[ 2], 18);

        x[ 6] ^= hashing_utils::rotl32(x[ 5] + x[ 4], 7);
        x[ 7] ^= hashing_utils::rotl32(x[ 6] + x[ 5], 9);
        x[ 4] ^= hashing_utils::rotl32(x[ 7] + x[ 6], 13);
        x[ 5] ^= hashing_utils::rotl32(x[ 4] + x[ 7], 18);

        x[11] ^= hashing_utils::rotl32(x[10] + x[ 9], 7);
        x[ 8] ^= hashing_utils::rotl32(x[11] + x[10], 9);
        x[ 9] ^= hashing_utils::rotl32(x[ 8] + x[11], 13);
        x[10] ^= hashing_utils::rotl32(x[ 9] + x[ 8], 18);

        x[12] ^= hashing_utils::rotl32(x[15] + x[14], 7);
        x[13] ^= hashing_utils::rotl32(x[12] + x[15], 9);
        x[14] ^= hashing_utils::rotl32(x[13] + x[12], 13);
        x[15] ^= hashing_utils::rotl32(x[14] + x[13], 18);
    }

    for (int i = 0; i < 16; ++i) {
        hashing_utils::store_le32(b + i * 4, b32[i] + x[i]);
    }
}

void block_mix(const unsigned char* b, unsigned char* y, std::uint32_t r) {
    unsigned char x[64];
    std::memcpy(x, b + (2 * static_cast<std::size_t>(r) - 1) * 64, 64);

    for (std::uint32_t i = 0; i < 2 * r; ++i) {
        for (int j = 0; j < 64; ++j) {
            x[j] ^= b[static_cast<std::size_t>(i) * 64 + j];
        }
        salsa20_8(x);
        std::size_t dest = (i % 2 == 0) ? (i / 2) : (r + i / 2);
        std::memcpy(y + dest * 64, x, 64);
    }
}

void romix(unsigned char* b, std::uint32_t r, std::uint32_t n) {
    std::size_t block_size = 128 * static_cast<std::size_t>(r);
    std::vector<unsigned char> v(block_size * static_cast<std::size_t>(n));
    std::vector<unsigned char> y(block_size);

    for (std::uint32_t i = 0; i < n; ++i) {
        std::memcpy(&v[static_cast<std::size_t>(i) * block_size], b, block_size);
        block_mix(b, y.data(), r);
        std::memcpy(b, y.data(), block_size);
    }

    for (std::uint32_t i = 0; i < n; ++i) {
        std::uint64_t j = hashing_utils::load_le64(b + (2 * static_cast<std::size_t>(r) - 1) * 64) & (n - 1);
        for (std::size_t k = 0; k < block_size; ++k) {
            b[k] ^= v[static_cast<std::size_t>(j) * block_size + k];
        }
        block_mix(b, y.data(), r);
        std::memcpy(b, y.data(), block_size);
    }
}

} // namespace

// RFC 7914 scrypt key derivation (default salt = "salt", N = 16384, r = 8, p = 1, dkLen = 32).
std::string scrypt_hash(const std::string& value, const std::string& salt,
                        std::uint32_t n, std::uint32_t r, std::uint32_t p, std::size_t dk_len) {
    if (r == 0 || p == 0 || dk_len == 0 || n < 2 || (n & (n - 1)) != 0) {
        throw std::invalid_argument("Invalid scrypt parameters: N must be a power of 2 > 1, and r, p, dk_len must be > 0.");
    }
    if (static_cast<std::uint64_t>(r) * static_cast<std::uint64_t>(p) >= (1ULL << 30)) {
        throw std::invalid_argument("scrypt parameters r * p exceed RFC 7914 limit.");
    }
    if (r == 1 && n >= 65536) {
        throw std::invalid_argument("scrypt parameter N must be < 2^(16 * r).");
    }

    std::size_t block_size = 128 * static_cast<std::size_t>(r);
    if (block_size > std::numeric_limits<std::size_t>::max() / n ||
        block_size > std::numeric_limits<std::size_t>::max() / p) {
        throw std::invalid_argument("scrypt memory allocation size overflow.");
    }

    auto b = hashing_utils::pbkdf2_raw(
        hashing_utils::sha256_raw, 64, 32, value, salt, 1,
        static_cast<std::size_t>(p) * block_size);

    for (std::uint32_t i = 0; i < p; ++i) {
        romix(&b[static_cast<std::size_t>(i) * block_size], r, n);
    }

    std::string b_str(reinterpret_cast<const char*>(b.data()), b.size());
    auto dk = hashing_utils::pbkdf2_raw(hashing_utils::sha256_raw, 64, 32, value, b_str, 1, dk_len);
    return hashing_utils::bytes_to_hex(dk);
}

std::string scrypt_hash(const std::string& value) {
    return scrypt_hash(value, "salt", 16384, 8, 1, 32);
}
