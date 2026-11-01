#include <cstdint>
#include <string>
#include "crc32_hash.h"
#include "hashing_utils.h"

// IEEE 802.3 CRC-32 checksum formatted as 8 lowercase hex digits.
std::string crc32_hash(const std::string& value) {
    std::uint32_t crc = 0xFFFFFFFFU;

    for (unsigned char byte : value) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xEDB88320U & (0U - (crc & 1U)));
        }
    }

    return hashing_utils::u32_to_hex(~crc);
}

