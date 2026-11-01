#include <cstdint>
#include <string>
#include "mysql323_hash.h"
#include "hashing_utils.h"

// MySQL 3.23 OLD_PASSWORD() 16-hex-character hash.
std::string mysql323_hash(const std::string& value) {
    std::uint32_t nr = 1345345333UL;
    std::uint32_t add = 7;
    std::uint32_t nr2 = 0x12345671UL;

    for (unsigned char c : value) {
        if (c == ' ' || c == '\t') {
            continue;
        }
        nr ^= (((nr & 63) + add) * c) + (nr << 8);
        nr2 += (nr2 << 8) ^ nr;
        add += c;
    }

    std::uint32_t res1 = nr & 0x7FFFFFFFUL;
    std::uint32_t res2 = nr2 & 0x7FFFFFFFUL;

    return hashing_utils::u32_to_hex(res1) + hashing_utils::u32_to_hex(res2);
}

