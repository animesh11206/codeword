#include <cstdint>
#include <string>
#include "xxhash64_hash.h"
#include "hashing_utils.h"

namespace {

constexpr std::uint64_t PRIME64_1 = 0x9E3779B185EBCA87ULL;
constexpr std::uint64_t PRIME64_2 = 0xC2B2AE3D27D4EB4FULL;
constexpr std::uint64_t PRIME64_3 = 0x165667B19E3779F9ULL;
constexpr std::uint64_t PRIME64_4 = 0x85EBCA77C2B2AE63ULL;
constexpr std::uint64_t PRIME64_5 = 0x27D4EB2F165667C5ULL;

std::uint64_t round64(std::uint64_t acc, std::uint64_t input) {
    acc += input * PRIME64_2;
    acc = hashing_utils::rotl64(acc, 31);
    acc *= PRIME64_1;
    return acc;
}

std::uint64_t merge_round64(std::uint64_t acc, std::uint64_t val) {
    val = round64(0, val);
    acc ^= val;
    acc = acc * PRIME64_1 + PRIME64_4;
    return acc;
}

} // namespace

// xxHash64 (seed = 0) formatted as 16 lowercase hex digits.
std::string xxhash64_hash(const std::string& value) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(value.data());
    const unsigned char* end = p + value.size();
    std::uint64_t h64 = 0;

    if (value.size() >= 32) {
        const unsigned char* limit = end - 32;
        std::uint64_t v1 = PRIME64_1 + PRIME64_2;
        std::uint64_t v2 = PRIME64_2;
        std::uint64_t v3 = 0;
        std::uint64_t v4 = 0ULL - PRIME64_1;

        do {
            v1 = round64(v1, hashing_utils::load_le64(p)); p += 8;
            v2 = round64(v2, hashing_utils::load_le64(p)); p += 8;
            v3 = round64(v3, hashing_utils::load_le64(p)); p += 8;
            v4 = round64(v4, hashing_utils::load_le64(p)); p += 8;
        } while (p <= limit);

        h64 = hashing_utils::rotl64(v1, 1) + hashing_utils::rotl64(v2, 7) +
              hashing_utils::rotl64(v3, 12) + hashing_utils::rotl64(v4, 18);
        h64 = merge_round64(h64, v1);
        h64 = merge_round64(h64, v2);
        h64 = merge_round64(h64, v3);
        h64 = merge_round64(h64, v4);
    }
    else {
        h64 = PRIME64_5;
    }

    h64 += static_cast<std::uint64_t>(value.size());

    while (p + 8 <= end) {
        std::uint64_t k1 = round64(0, hashing_utils::load_le64(p));
        h64 ^= k1;
        h64 = hashing_utils::rotl64(h64, 27) * PRIME64_1 + PRIME64_4;
        p += 8;
    }

    if (p + 4 <= end) {
        h64 ^= static_cast<std::uint64_t>(hashing_utils::load_le32(p)) * PRIME64_1;
        h64 = hashing_utils::rotl64(h64, 23) * PRIME64_2 + PRIME64_3;
        p += 4;
    }

    while (p < end) {
        h64 ^= static_cast<std::uint64_t>(*p++) * PRIME64_5;
        h64 = hashing_utils::rotl64(h64, 11) * PRIME64_1;
    }

    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;

    return hashing_utils::u64_to_hex(h64);
}

