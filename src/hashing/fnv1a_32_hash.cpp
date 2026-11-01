#include <cstdint>
#include <string>
#include "fnv1a_32_hash.h"
#include "hashing_utils.h"

// 32-bit FNV-1a hash formatted as 8 lowercase hex digits.
std::string fnv1a_32_hash(const std::string& value) {
    std::uint32_t hash = 0x811C9DC5U;

    for (unsigned char byte : value) {
        hash ^= byte;
        hash *= 0x01000193U;
    }

    return hashing_utils::u32_to_hex(hash);
}

