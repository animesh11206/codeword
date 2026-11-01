#include <cstdint>
#include <string>
#include "fnv1a_64_hash.h"
#include "hashing_utils.h"

// 64-bit FNV-1a hash formatted as 16 lowercase hex digits.
std::string fnv1a_64_hash(const std::string& value) {
    std::uint64_t hash = 0xCBF29CE484222325ULL;

    for (unsigned char byte : value) {
        hash ^= byte;
        hash *= 0x00000100000001B3ULL;
    }

    return hashing_utils::u64_to_hex(hash);
}

