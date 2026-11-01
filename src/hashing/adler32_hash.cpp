#include <cstdint>
#include <string>
#include "adler32_hash.h"
#include "hashing_utils.h"

// RFC 1950 Adler-32 checksum formatted as 8 lowercase hex digits.
std::string adler32_hash(const std::string& value) {
    static constexpr std::uint32_t MOD_ADLER = 65521;

    std::uint32_t a = 1;
    std::uint32_t b = 0;

    for (unsigned char byte : value) {
        a = (a + byte) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return hashing_utils::u32_to_hex((b << 16) | a);
}

