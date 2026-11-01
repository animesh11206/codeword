#include <cstdint>
#include <string>
#include <vector>
#include "sm3_hash.h"
#include "hashing_utils.h"

namespace {

std::uint32_t sm3_p0(std::uint32_t x) {
    return x ^ hashing_utils::rotl32(x, 9) ^ hashing_utils::rotl32(x, 17);
}

std::uint32_t sm3_p1(std::uint32_t x) {
    return x ^ hashing_utils::rotl32(x, 15) ^ hashing_utils::rotl32(x, 23);
}

std::uint32_t sm3_ff(int j, std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return (j < 16) ? (x ^ y ^ z) : ((x & y) | (x & z) | (y & z));
}

std::uint32_t sm3_gg(int j, std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return (j < 16) ? (x ^ y ^ z) : ((x & y) | (~x & z));
}

} // namespace

// GM/T 0004-2012 SM3 256-bit cryptographic hash (64 lowercase hex characters).
std::string sm3_hash(const std::string& value) {
    std::vector<unsigned char> msg(value.begin(), value.end());
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    unsigned char len_bytes[8];
    hashing_utils::store_be64(len_bytes, static_cast<std::uint64_t>(value.size()) * 8);
    msg.insert(msg.end(), len_bytes, len_bytes + 8);

    std::uint32_t v[8] = {
        0x7380166FU, 0x4914B2B9U, 0x172442D7U, 0xDA8A0600U,
        0xA96F30BCU, 0x163138AAU, 0xE38DEE4DU, 0xB0FB0E4EU
    };

    for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
        std::uint32_t w[68];
        std::uint32_t w1[64];

        for (int i = 0; i < 16; ++i) {
            w[i] = hashing_utils::load_be32(&msg[offset + i * 4]);
        }
        for (int i = 16; i < 68; ++i) {
            w[i] = sm3_p1(w[i - 16] ^ w[i - 9] ^ hashing_utils::rotl32(w[i - 3], 15)) ^
                   hashing_utils::rotl32(w[i - 13], 7) ^ w[i - 6];
        }
        for (int i = 0; i < 64; ++i) {
            w1[i] = w[i] ^ w[i + 4];
        }

        std::uint32_t a = v[0], b = v[1], c = v[2], d = v[3];
        std::uint32_t e = v[4], f = v[5], g = v[6], h = v[7];

        for (int j = 0; j < 64; ++j) {
            std::uint32_t tj = (j < 16) ? 0x79CC4519U : 0x7A879D8AU;
            std::uint32_t ss1 = hashing_utils::rotl32(
                hashing_utils::rotl32(a, 12) + e + hashing_utils::rotl32(tj, j & 31), 7);
            std::uint32_t ss2 = ss1 ^ hashing_utils::rotl32(a, 12);
            std::uint32_t tt1 = sm3_ff(j, a, b, c) + d + ss2 + w1[j];
            std::uint32_t tt2 = sm3_gg(j, e, f, g) + h + ss1 + w[j];

            d = c;
            c = hashing_utils::rotl32(b, 9);
            b = a;
            a = tt1;
            h = g;
            g = hashing_utils::rotl32(f, 19);
            f = e;
            e = sm3_p0(tt2);
        }

        v[0] ^= a; v[1] ^= b; v[2] ^= c; v[3] ^= d;
        v[4] ^= e; v[5] ^= f; v[6] ^= g; v[7] ^= h;
    }

    unsigned char out[32];
    for (int i = 0; i < 8; ++i) {
        hashing_utils::store_be32(out + i * 4, v[i]);
    }
    return hashing_utils::bytes_to_hex(out, 32);
}

