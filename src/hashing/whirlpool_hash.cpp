#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include "hashing_utils.h"
#include "whirlpool_hash.h"

namespace {

struct WhirlpoolTables {
    std::uint8_t sbox[256];
    std::uint64_t c_table[8][256];
    std::uint64_t rc[11];

    static std::uint8_t gf_mul(std::uint8_t a, std::uint8_t b) {
        std::uint8_t res = 0;
        while (b > 0) {
            if ((b & 1U) != 0) {
                res ^= a;
            }
            bool hi = (a & 0x80U) != 0;
            a = static_cast<std::uint8_t>(a << 1);
            if (hi) {
                a ^= 0x1DU; // x^8 + x^4 + x^3 + x^2 + 1 (0x11D)
            }
            b >>= 1;
        }
        return res;
    }

    WhirlpoolTables() : sbox{}, c_table{}, rc{} {
        static constexpr std::uint8_t E[16] = {
            0x1, 0xB, 0x9, 0xC, 0xD, 0x6, 0xF, 0x3,
            0xE, 0x8, 0x7, 0x4, 0xA, 0x2, 0x5, 0x0
        };
        static constexpr std::uint8_t E_inv[16] = {
            0xF, 0x0, 0xD, 0x7, 0xB, 0xE, 0x5, 0xA,
            0x9, 0x2, 0xC, 0x1, 0x3, 0x4, 0x8, 0x6
        };
        static constexpr std::uint8_t R[16] = {
            0x7, 0xC, 0xB, 0xD, 0xE, 0x4, 0x9, 0xF,
            0x6, 0x3, 0x8, 0xA, 0x2, 0x5, 0x1, 0x0
        };

        for (int x = 0; x < 256; ++x) {
            std::uint8_t u = static_cast<std::uint8_t>(x >> 4);
            std::uint8_t v = static_cast<std::uint8_t>(x & 0x0F);
            std::uint8_t a = E[u];
            std::uint8_t b = E_inv[v];
            std::uint8_t r = R[a ^ b];
            std::uint8_t u2 = E[a ^ r];
            std::uint8_t v2 = E_inv[b ^ r];
            sbox[x] = static_cast<std::uint8_t>((u2 << 4) | v2);
        }

        // Circulant MDS matrix first row: [1, 1, 4, 1, 8, 5, 2, 9]
        static constexpr std::uint8_t C0[8] = {1, 1, 4, 1, 8, 5, 2, 9};
        for (int t = 0; t < 8; ++t) {
            for (int x = 0; x < 256; ++x) {
                std::uint8_t s = sbox[x];
                std::uint64_t word = 0;
                for (int col = 0; col < 8; ++col) {
                    std::uint8_t coeff = C0[(col - t + 8) & 7];
                    std::uint8_t prod = gf_mul(s, coeff);
                    word = (word << 8) | prod;
                }
                c_table[t][x] = word;
            }
        }

        rc[0] = 0;
        for (int r = 1; r <= 10; ++r) {
            std::uint64_t val = 0;
            for (int j = 0; j < 8; ++j) {
                val = (val << 8) | sbox[8 * (r - 1) + j];
            }
            rc[r] = val;
        }
    }
};

const WhirlpoolTables& get_whirlpool_tables() {
    static const WhirlpoolTables tables;
    return tables;
}

void whirlpool_compress(std::uint64_t state[8], const std::uint8_t block[64]) {
    const WhirlpoolTables& T = get_whirlpool_tables();

    std::uint64_t K[8];
    std::uint64_t M[8];
    std::uint64_t W[8];

    for (int i = 0; i < 8; ++i) {
        K[i] = state[i];
        M[i] = hashing_utils::load_be64(block + i * 8);
        W[i] = M[i] ^ K[i];
    }

    for (int r = 1; r <= 10; ++r) {
        std::uint64_t next_K[8] = {};
        next_K[0] = T.rc[r];

        for (int i = 0; i < 8; ++i) {
            for (int t = 0; t < 8; ++t) {
                std::uint8_t byte_val = static_cast<std::uint8_t>(
                    (K[(i - t + 8) & 7] >> (56 - 8 * t)) & 0xFFU);
                next_K[i] ^= T.c_table[t][byte_val];
            }
        }
        for (int i = 0; i < 8; ++i) {
            K[i] = next_K[i];
        }

        std::uint64_t next_W[8];
        for (int i = 0; i < 8; ++i) {
            next_W[i] = K[i];
            for (int t = 0; t < 8; ++t) {
                std::uint8_t byte_val = static_cast<std::uint8_t>(
                    (W[(i - t + 8) & 7] >> (56 - 8 * t)) & 0xFFU);
                next_W[i] ^= T.c_table[t][byte_val];
            }
        }
        for (int i = 0; i < 8; ++i) {
            W[i] = next_W[i];
        }
    }

    for (int i = 0; i < 8; ++i) {
        state[i] ^= W[i] ^ M[i];
    }
}

} // namespace

std::string whirlpool_hash(const std::string& value) {
    std::uint64_t state[8] = {};

    const auto* data = reinterpret_cast<const std::uint8_t*>(value.data());
    std::size_t len = value.size();
    std::size_t offset = 0;

    while (offset + 64 <= len) {
        whirlpool_compress(state, data + offset);
        offset += 64;
    }

    std::uint8_t tail[128] = {};
    std::size_t rem = len - offset;
    if (rem > 0) {
        std::memcpy(tail, data + offset, rem);
    }
    tail[rem] = 0x80U;

    std::size_t total_len = (rem < 32) ? 64 : 128;
    std::uint64_t bit_len = static_cast<std::uint64_t>(len) * 8ULL;
    // 256-bit big-endian bit length in the last 32 bytes; high 24 bytes are 0
    hashing_utils::store_be64(tail + total_len - 8, bit_len);

    whirlpool_compress(state, tail);
    if (total_len == 128) {
        whirlpool_compress(state, tail + 64);
    }

    std::uint8_t digest[64];
    for (int i = 0; i < 8; ++i) {
        hashing_utils::store_be64(digest + i * 8, state[i]);
    }
    return hashing_utils::bytes_to_hex(digest, 64);
}
