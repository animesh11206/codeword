#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "ripemd256_hash.h"
#include "hashing_utils.h"

namespace {

constexpr int R256_RL[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
    3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
    1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2
};

constexpr int R256_RR[64] = {
    5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
    6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
    15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
    8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14
};

constexpr unsigned R256_SL[64] = {
    11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
    7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
    11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
    11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12
};

constexpr unsigned R256_SR[64] = {
    8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
    9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
    9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
    15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8
};

std::uint32_t r256_f(int j, std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    if (j < 16) return x ^ y ^ z;
    if (j < 32) return (x & y) | (~x & z);
    if (j < 48) return (x | ~y) ^ z;
    return (x & z) | (y & ~z);
}

std::uint32_t r256_kl(int j) {
    if (j < 16) return 0x00000000U;
    if (j < 32) return 0x5A827999U;
    if (j < 48) return 0x6ED9EBA1U;
    return 0x8F1BBCDCU;
}

std::uint32_t r256_kr(int j) {
    if (j < 16) return 0x50A28BE6U;
    if (j < 32) return 0x5C4DD124U;
    if (j < 48) return 0x6D703EF3U;
    return 0x00000000U;
}

} // namespace

// RIPEMD-256 message digest (64 lowercase hex characters).
std::string ripemd256_hash(const std::string& value) {
    std::vector<unsigned char> msg(value.begin(), value.end());
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) msg.push_back(0x00);

    unsigned char len_bytes[8];
    hashing_utils::store_le64(len_bytes, static_cast<std::uint64_t>(value.size()) * 8);
    msg.insert(msg.end(), len_bytes, len_bytes + 8);

    std::uint32_t h[8] = {
        0x67452301U, 0xEFCDAB89U, 0x98BADCFEU, 0x10325476U,
        0x76543210U, 0xFEDCBA98U, 0x89ABCDEFU, 0x01234567U
    };

    for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
        std::uint32_t x[16];
        for (int i = 0; i < 16; ++i) x[i] = hashing_utils::load_le32(&msg[offset + i * 4]);

        std::uint32_t al = h[0], bl = h[1], cl = h[2], dl = h[3];
        std::uint32_t ar = h[4], br = h[5], cr = h[6], dr = h[7];

        for (int j = 0; j < 64; ++j) {
            std::uint32_t t = hashing_utils::rotl32(al + r256_f(j, bl, cl, dl) + x[R256_RL[j]] + r256_kl(j), R256_SL[j]);
            al = dl; dl = cl; cl = bl; bl = t;

            t = hashing_utils::rotl32(ar + r256_f(63 - j, br, cr, dr) + x[R256_RR[j]] + r256_kr(j), R256_SR[j]);
            ar = dr; dr = cr; cr = br; br = t;

            if (j == 15) std::swap(al, ar);
            else if (j == 31) std::swap(bl, br);
            else if (j == 47) std::swap(cl, cr);
            else if (j == 63) std::swap(dl, dr);
        }

        h[0] += al; h[1] += bl; h[2] += cl; h[3] += dl;
        h[4] += ar; h[5] += br; h[6] += cr; h[7] += dr;
    }

    unsigned char out[32];
    for (int i = 0; i < 8; ++i) hashing_utils::store_le32(out + i * 4, h[i]);
    return hashing_utils::bytes_to_hex(out, 32);
}

