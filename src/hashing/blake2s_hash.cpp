#include <cstdint>
#include <cstring>
#include <string>
#include "blake2s_hash.h"
#include "hashing_utils.h"

namespace {

constexpr std::uint32_t BLAKE2S_IV[8] = {
    0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU,
    0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U
};

constexpr unsigned char BLAKE2S_SIGMA[10][16] = {
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
    {14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3},
    {11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4},
    { 7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8},
    { 9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13},
    { 2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9},
    {12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11},
    {13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10},
    { 6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5},
    {10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13,  0}
};

void blake2s_g(std::uint32_t v[16], int a, int b, int c, int d, std::uint32_t x, std::uint32_t y) {
    v[a] = v[a] + v[b] + x;
    v[d] = hashing_utils::rotr32(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];
    v[b] = hashing_utils::rotr32(v[b] ^ v[c], 12);
    v[a] = v[a] + v[b] + y;
    v[d] = hashing_utils::rotr32(v[d] ^ v[a], 8);
    v[c] = v[c] + v[d];
    v[b] = hashing_utils::rotr32(v[b] ^ v[c], 7);
}

void blake2s_compress(std::uint32_t h[8], const unsigned char block[64], std::uint64_t t, bool last) {
    std::uint32_t m[16];
    std::uint32_t v[16];

    for (int i = 0; i < 16; ++i) {
        m[i] = hashing_utils::load_le32(block + i * 4);
    }
    for (int i = 0; i < 8; ++i) {
        v[i] = h[i];
        v[i + 8] = BLAKE2S_IV[i];
    }

    v[12] ^= static_cast<std::uint32_t>(t & 0xFFFFFFFFULL);
    v[13] ^= static_cast<std::uint32_t>(t >> 32);
    if (last) {
        v[14] = ~v[14];
    }

    for (int r = 0; r < 10; ++r) {
        const unsigned char* s = BLAKE2S_SIGMA[r];
        blake2s_g(v, 0, 4,  8, 12, m[s[0]],  m[s[1]]);
        blake2s_g(v, 1, 5,  9, 13, m[s[2]],  m[s[3]]);
        blake2s_g(v, 2, 6, 10, 14, m[s[4]],  m[s[5]]);
        blake2s_g(v, 3, 7, 11, 15, m[s[6]],  m[s[7]]);
        blake2s_g(v, 0, 5, 10, 15, m[s[8]],  m[s[9]]);
        blake2s_g(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        blake2s_g(v, 2, 7,  8, 13, m[s[12]], m[s[13]]);
        blake2s_g(v, 3, 4,  9, 14, m[s[14]], m[s[15]]);
    }

    for (int i = 0; i < 8; ++i) {
        h[i] ^= v[i] ^ v[i + 8];
    }
}

} // namespace

// RFC 7693 BLAKE2s-256 (64 lowercase hex characters).
std::string blake2s_hash(const std::string& value) {
    std::uint32_t h[8];
    for (int i = 0; i < 8; ++i) {
        h[i] = BLAKE2S_IV[i];
    }
    h[0] ^= 0x01010020U; // fanout=1, depth=1, outlen=32

    std::size_t offset = 0;
    std::uint64_t total = 0;

    while (offset + 64 < value.size()) {
        total += 64;
        blake2s_compress(h, reinterpret_cast<const unsigned char*>(value.data() + offset), total, false);
        offset += 64;
    }

    unsigned char final_block[64] = {0};
    std::size_t rem = value.size() - offset;
    if (rem > 0) {
        std::memcpy(final_block, value.data() + offset, rem);
    }
    total += rem;
    blake2s_compress(h, final_block, total, true);

    unsigned char out[32];
    for (int i = 0; i < 8; ++i) {
        hashing_utils::store_le32(out + i * 4, h[i]);
    }
    return hashing_utils::bytes_to_hex(out, 32);
}

