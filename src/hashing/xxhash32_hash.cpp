#include <cstdint>
#include <string>
#include "xxhash32_hash.h"
#include "hashing_utils.h"

// xxHash32 (seed = 0) formatted as 8 lowercase hex digits.
std::string xxhash32_hash(const std::string& value) {
    static constexpr std::uint32_t PRIME1 = 0x9E3779B1U;
    static constexpr std::uint32_t PRIME2 = 0x85EBCA77U;
    static constexpr std::uint32_t PRIME3 = 0xC2B2AE3DU;
    static constexpr std::uint32_t PRIME4 = 0x27D4EB2FU;
    static constexpr std::uint32_t PRIME5 = 0x165667B1U;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(value.data());
    const unsigned char* end = p + value.size();
    std::uint32_t h32 = 0;

    if (value.size() >= 16) {
        const unsigned char* limit = end - 16;
        std::uint32_t v1 = PRIME1 + PRIME2;
        std::uint32_t v2 = PRIME2;
        std::uint32_t v3 = 0;
        std::uint32_t v4 = 0U - PRIME1;

        do {
            v1 = hashing_utils::rotl32(v1 + hashing_utils::load_le32(p) * PRIME2, 13) * PRIME1; p += 4;
            v2 = hashing_utils::rotl32(v2 + hashing_utils::load_le32(p) * PRIME2, 13) * PRIME1; p += 4;
            v3 = hashing_utils::rotl32(v3 + hashing_utils::load_le32(p) * PRIME2, 13) * PRIME1; p += 4;
            v4 = hashing_utils::rotl32(v4 + hashing_utils::load_le32(p) * PRIME2, 13) * PRIME1; p += 4;
        } while (p <= limit);

        h32 = hashing_utils::rotl32(v1, 1) + hashing_utils::rotl32(v2, 7) +
              hashing_utils::rotl32(v3, 12) + hashing_utils::rotl32(v4, 18);
    }
    else {
        h32 = PRIME5;
    }

    h32 += static_cast<std::uint32_t>(value.size());

    while (p + 4 <= end) {
        h32 += hashing_utils::load_le32(p) * PRIME3;
        h32 = hashing_utils::rotl32(h32, 17) * PRIME4;
        p += 4;
    }

    while (p < end) {
        h32 += static_cast<std::uint32_t>(*p++) * PRIME5;
        h32 = hashing_utils::rotl32(h32, 11) * PRIME1;
    }

    h32 ^= h32 >> 15;
    h32 *= PRIME2;
    h32 ^= h32 >> 13;
    h32 *= PRIME3;
    h32 ^= h32 >> 16;

    return hashing_utils::u32_to_hex(h32);
}

