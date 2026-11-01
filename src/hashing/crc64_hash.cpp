#include <cstdint>
#include <string>
#include "crc64_hash.h"
#include "hashing_utils.h"

// CRC-64-XZ (reflected ECMA-182 polynomial 0xC96C5795D7870F42) formatted as 16 lowercase hex digits.
std::string crc64_hash(const std::string& value) {
    std::uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;

    for (unsigned char byte : value) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xC96C5795D7870F42ULL & (0ULL - (crc & 1ULL)));
        }
    }

    return hashing_utils::u64_to_hex(~crc);
}

