#include <cstdint>
#include <string>
#include <vector>
#include "ripemd160_hash.h"
#include "hashing_utils.h"

namespace {

constexpr int R160_RL[80] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
    3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
    1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
    4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13
};

constexpr int R160_RR[80] = {
    5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
    6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
    15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
    8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
    12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11
};

constexpr unsigned R160_SL[80] = {
    11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
    7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
    11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
    11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
    9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6
};

constexpr unsigned R160_SR[80] = {
    8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
    9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
    9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
    15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
    8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11
};

std::uint32_t r160_f(int j, std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    if (j < 16) return x ^ y ^ z;
    if (j < 32) return (x & y) | (~x & z);
    if (j < 48) return (x | ~y) ^ z;
    if (j < 64) return (x & z) | (y & ~z);
    return x ^ (y | ~z);
}

std::uint32_t r160_kl(int j) {
    if (j < 16) return 0x00000000U;
    if (j < 32) return 0x5A827999U;
    if (j < 48) return 0x6ED9EBA1U;
    if (j < 64) return 0x8F1BBCDCU;
    return 0xA953FD4EU;
}

std::uint32_t r160_kr(int j) {
    if (j < 16) return 0x50A28BE6U;
    if (j < 32) return 0x5C4DD124U;
    if (j < 48) return 0x6D703EF3U;
    if (j < 64) return 0x7A6D76E9U;
    return 0x00000000U;
}

} // namespace

// RIPEMD-160 message digest (40 lowercase hex characters).
std::string ripemd160_hash(const std::string& value) {
    std::vector<unsigned char> msg(value.begin(), value.end());
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) msg.push_back(0x00);

    unsigned char len_bytes[8];
    hashing_utils::store_le64(len_bytes, static_cast<std::uint64_t>(value.size()) * 8);
    msg.insert(msg.end(), len_bytes, len_bytes + 8);

    std::uint32_t h0 = 0x67452301U, h1 = 0xEFCDAB89U, h2 = 0x98BADCFEU, h3 = 0x10325476U, h4 = 0xC3D2E1F0U;

    for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
        std::uint32_t x[16];
        for (int i = 0; i < 16; ++i) x[i] = hashing_utils::load_le32(&msg[offset + i * 4]);

        std::uint32_t al = h0, bl = h1, cl = h2, dl = h3, el = h4;
        std::uint32_t ar = h0, br = h1, cr = h2, dr = h3, er = h4;

        for (int j = 0; j < 80; ++j) {
            std::uint32_t t = hashing_utils::rotl32(al + r160_f(j, bl, cl, dl) + x[R160_RL[j]] + r160_kl(j), R160_SL[j]) + el;
            al = el; el = dl; dl = hashing_utils::rotl32(cl, 10); cl = bl; bl = t;

            t = hashing_utils::rotl32(ar + r160_f(79 - j, br, cr, dr) + x[R160_RR[j]] + r160_kr(j), R160_SR[j]) + er;
            ar = er; er = dr; dr = hashing_utils::rotl32(cr, 10); cr = br; br = t;
        }

        std::uint32_t t = h1 + cl + dr;
        h1 = h2 + dl + er;
        h2 = h3 + el + ar;
        h3 = h4 + al + br;
        h4 = h0 + bl + cr;
        h0 = t;
    }

    unsigned char out[20];
    hashing_utils::store_le32(out, h0);
    hashing_utils::store_le32(out + 4, h1);
    hashing_utils::store_le32(out + 8, h2);
    hashing_utils::store_le32(out + 12, h3);
    hashing_utils::store_le32(out + 16, h4);
    return hashing_utils::bytes_to_hex(out, 20);
}

