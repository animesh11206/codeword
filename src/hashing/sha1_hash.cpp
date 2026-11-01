#include <cstdint>
#include <string>
#include <vector>
#include "sha1_hash.h"
#include "hashing_utils.h"

namespace hashing_utils {

std::vector<unsigned char> sha1_raw(const unsigned char* data, std::size_t len) {
    std::vector<unsigned char> msg(data, data + len);
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    std::uint64_t bit_len = static_cast<std::uint64_t>(len) * 8;
    unsigned char len_bytes[8];
    store_be64(len_bytes, bit_len);
    msg.insert(msg.end(), len_bytes, len_bytes + 8);

    std::uint32_t h0 = 0x67452301U;
    std::uint32_t h1 = 0xEFCDAB89U;
    std::uint32_t h2 = 0x98BADCFEU;
    std::uint32_t h3 = 0x10325476U;
    std::uint32_t h4 = 0xC3D2E1F0U;

    for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
        std::uint32_t w[80];
        for (int i = 0; i < 16; ++i) {
            w[i] = load_be32(&msg[offset + i * 4]);
        }
        for (int i = 16; i < 80; ++i) {
            w[i] = rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        std::uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;

        for (int i = 0; i < 80; ++i) {
            std::uint32_t f = 0, k = 0;
            if (i < 20) {
                f = (b & c) | (~b & d);
                k = 0x5A827999U;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1U;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDCU;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6U;
            }

            std::uint32_t temp = rotl32(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotl32(b, 30);
            b = a;
            a = temp;
        }

        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }

    std::vector<unsigned char> digest(20);
    store_be32(&digest[0], h0);
    store_be32(&digest[4], h1);
    store_be32(&digest[8], h2);
    store_be32(&digest[12], h3);
    store_be32(&digest[16], h4);
    return digest;
}

} // namespace hashing_utils

// FIPS 180-4 SHA-1 digest (40 lowercase hex characters).
std::string sha1_hash(const std::string& value) {
    auto digest = hashing_utils::sha1_raw(reinterpret_cast<const unsigned char*>(value.data()), value.size());
    return hashing_utils::bytes_to_hex(digest);
}

