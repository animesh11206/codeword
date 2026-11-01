#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include "hashing_utils.h"
#include "tiger192_hash.h"

namespace {

inline void tiger_round(std::uint64_t& a, std::uint64_t& b, std::uint64_t& c,
                        std::uint64_t x, std::uint64_t mul,
                        const std::uint64_t T[1024]) {
    c ^= x;
    a -= T[0 * 256 + static_cast<std::size_t>((c >>  0) & 0xFFU)] ^
         T[1 * 256 + static_cast<std::size_t>((c >> 16) & 0xFFU)] ^
         T[2 * 256 + static_cast<std::size_t>((c >> 32) & 0xFFU)] ^
         T[3 * 256 + static_cast<std::size_t>((c >> 48) & 0xFFU)];
    b += T[3 * 256 + static_cast<std::size_t>((c >>  8) & 0xFFU)] ^
         T[2 * 256 + static_cast<std::size_t>((c >> 24) & 0xFFU)] ^
         T[1 * 256 + static_cast<std::size_t>((c >> 40) & 0xFFU)] ^
         T[0 * 256 + static_cast<std::size_t>((c >> 56) & 0xFFU)];
    b *= mul;
}

inline void tiger_pass(std::uint64_t& a, std::uint64_t& b, std::uint64_t& c,
                       const std::uint64_t x[8], std::uint64_t mul,
                       const std::uint64_t T[1024]) {
    tiger_round(a, b, c, x[0], mul, T);
    tiger_round(b, c, a, x[1], mul, T);
    tiger_round(c, a, b, x[2], mul, T);
    tiger_round(a, b, c, x[3], mul, T);
    tiger_round(b, c, a, x[4], mul, T);
    tiger_round(c, a, b, x[5], mul, T);
    tiger_round(a, b, c, x[6], mul, T);
    tiger_round(b, c, a, x[7], mul, T);
}

inline void tiger_key_schedule(std::uint64_t x[8]) {
    x[0] -= x[7] ^ 0xA5A5A5A5A5A5A5A5ULL;
    x[1] ^= x[0];
    x[2] += x[1];
    x[3] -= x[2] ^ ((~x[1]) << 19);
    x[4] ^= x[3];
    x[5] += x[4];
    x[6] -= x[5] ^ ((~x[4]) >> 23);
    x[7] ^= x[6];
    x[0] += x[7];
    x[1] -= x[0] ^ ((~x[7]) << 19);
    x[2] ^= x[1];
    x[3] += x[2];
    x[4] -= x[3] ^ ((~x[2]) >> 23);
    x[5] ^= x[4];
    x[6] += x[5];
    x[7] -= x[6] ^ 0x0123456789ABCDEFULL;
}

void tiger_compress(const std::uint64_t str[8], std::uint64_t state[3],
                    const std::uint64_t T[1024]) {
    std::uint64_t a = state[0];
    std::uint64_t b = state[1];
    std::uint64_t c = state[2];
    std::uint64_t aa = a;
    std::uint64_t bb = b;
    std::uint64_t cc = c;

    std::uint64_t x[8];
    for (int i = 0; i < 8; ++i) {
        x[i] = str[i];
    }

    tiger_pass(a, b, c, x, 5, T);
    tiger_key_schedule(x);
    tiger_pass(c, a, b, x, 7, T);
    tiger_key_schedule(x);
    tiger_pass(b, c, a, x, 9, T);

    a ^= aa;
    b -= bb;
    c += cc;

    state[0] = a;
    state[1] = b;
    state[2] = c;
}

struct TigerSBoxes {
    std::uint64_t t[1024];

    TigerSBoxes() : t{} {
        for (int i = 0; i < 1024; ++i) {
            t[i] = 0x0101010101010101ULL * static_cast<std::uint64_t>(i & 0xFF);
        }

        static constexpr char seed_phrase[65] =
            "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham";
        std::uint64_t temp_str[8];
        for (int i = 0; i < 8; ++i) {
            temp_str[i] = hashing_utils::load_le64(
                reinterpret_cast<const unsigned char*>(seed_phrase + i * 8));
        }

        std::uint64_t temp_abc[3] = {
            0x0123456789ABCDEFULL,
            0xFEDCBA9876543210ULL,
            0xF096A5B4C3B2E187ULL
        };

        int abc = 2;
        for (int cnt = 0; cnt < 5; ++cnt) {
            for (int i = 0; i < 256; ++i) {
                for (int sb = 0; sb < 1024; sb += 256) {
                    ++abc;
                    if (abc == 3) {
                        abc = 0;
                        tiger_compress(temp_str, temp_abc, t);
                    }
                    for (int col = 0; col < 8; ++col) {
                        std::uint64_t sh = static_cast<std::uint64_t>(8 * col);
                        std::uint64_t mask = 0xFFULL << sh;
                        int dest = sb + static_cast<int>((temp_abc[abc] >> sh) & 0xFFULL);
                        std::uint64_t bi = t[sb + i] & mask;
                        std::uint64_t bj = t[dest] & mask;
                        t[sb + i] = (t[sb + i] & ~mask) | bj;
                        t[dest] = (t[dest] & ~mask) | bi;
                    }
                }
            }
        }
    }
};

const std::uint64_t* get_tiger_sboxes() {
    static const TigerSBoxes sboxes;
    return sboxes.t;
}

} // namespace

std::string tiger192_hash(const std::string& value) {
    const std::uint64_t* T = get_tiger_sboxes();

    std::uint64_t state[3] = {
        0x0123456789ABCDEFULL,
        0xFEDCBA9876543210ULL,
        0xF096A5B4C3B2E187ULL
    };

    const auto* data = reinterpret_cast<const std::uint8_t*>(value.data());
    std::size_t len = value.size();
    std::size_t offset = 0;

    while (offset + 64 <= len) {
        std::uint64_t block_words[8];
        for (int i = 0; i < 8; ++i) {
            block_words[i] = hashing_utils::load_le64(data + offset + i * 8);
        }
        tiger_compress(block_words, state, T);
        offset += 64;
    }

    std::uint8_t tail[128] = {};
    std::size_t rem = len - offset;
    if (rem > 0) {
        std::memcpy(tail, data + offset, rem);
    }
    tail[rem] = 0x01U;

    std::size_t total_len = (rem < 56) ? 64 : 128;
    hashing_utils::store_le64(tail + total_len - 8, static_cast<std::uint64_t>(len) * 8ULL);

    for (std::size_t b = 0; b < total_len; b += 64) {
        std::uint64_t block_words[8];
        for (int i = 0; i < 8; ++i) {
            block_words[i] = hashing_utils::load_le64(tail + b + i * 8);
        }
        tiger_compress(block_words, state, T);
    }

    std::uint8_t digest[24];
    hashing_utils::store_le64(digest + 0, state[0]);
    hashing_utils::store_le64(digest + 8, state[1]);
    hashing_utils::store_le64(digest + 16, state[2]);
    return hashing_utils::bytes_to_hex(digest, 24);
}
