#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include "md4_hash.h"
#include "hashing_utils.h"

namespace hashing_utils {

std::vector<unsigned char> md4_raw(const unsigned char* data, std::size_t len) {
    std::vector<unsigned char> msg(data, data + len);
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    std::uint64_t bit_len = static_cast<std::uint64_t>(len) * 8;
    unsigned char len_bytes[8];
    store_le64(len_bytes, bit_len);
    msg.insert(msg.end(), len_bytes, len_bytes + 8);

    std::uint32_t a0 = 0x67452301U;
    std::uint32_t b0 = 0xEFCDAB89U;
    std::uint32_t c0 = 0x98BADCFEU;
    std::uint32_t d0 = 0x10325476U;

    auto f = [](std::uint32_t x, std::uint32_t y, std::uint32_t z) { return (x & y) | (~x & z); };
    auto g = [](std::uint32_t x, std::uint32_t y, std::uint32_t z) { return (x & y) | (x & z) | (y & z); };
    auto h = [](std::uint32_t x, std::uint32_t y, std::uint32_t z) { return x ^ y ^ z; };

    for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
        std::uint32_t x[16];
        for (int i = 0; i < 16; ++i) {
            x[i] = load_le32(&msg[offset + i * 4]);
        }

        std::uint32_t a = a0, b = b0, c = c0, d = d0;

        static constexpr int r1_s[4] = {3, 7, 11, 19};
        for (int i = 0; i < 16; ++i) {
            std::uint32_t tmp = rotl32(a + f(b, c, d) + x[i], r1_s[i & 3]);
            a = d; d = c; c = b; b = tmp;
        }

        static constexpr int r2_k[16] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
        static constexpr int r2_s[4] = {3, 5, 9, 13};
        for (int i = 0; i < 16; ++i) {
            std::uint32_t tmp = rotl32(a + g(b, c, d) + x[r2_k[i]] + 0x5A827999U, r2_s[i & 3]);
            a = d; d = c; c = b; b = tmp;
        }

        static constexpr int r3_k[16] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
        static constexpr int r3_s[4] = {3, 9, 11, 15};
        for (int i = 0; i < 16; ++i) {
            std::uint32_t tmp = rotl32(a + h(b, c, d) + x[r3_k[i]] + 0x6ED9EBA1U, r3_s[i & 3]);
            a = d; d = c; c = b; b = tmp;
        }

        a0 += a; b0 += b; c0 += c; d0 += d;
    }

    std::vector<unsigned char> digest(16);
    store_le32(&digest[0], a0);
    store_le32(&digest[4], b0);
    store_le32(&digest[8], c0);
    store_le32(&digest[12], d0);
    return digest;
}

} // namespace hashing_utils

// RFC 1320 MD4 message digest (32 lowercase hex characters).
std::string md4_hash(const std::string& value) {
    auto digest = hashing_utils::md4_raw(reinterpret_cast<const unsigned char*>(value.data()), value.size());
    return hashing_utils::bytes_to_hex(digest);
}

