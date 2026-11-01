#ifndef HASHING_UTILS_H
#define HASHING_UTILS_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace hashing_utils {

inline constexpr std::uint32_t rotl32(std::uint32_t x, unsigned n) {
    return (x << (n & 31)) | (x >> ((32 - (n & 31)) & 31));
}

inline constexpr std::uint32_t rotr32(std::uint32_t x, unsigned n) {
    return (x >> (n & 31)) | (x << ((32 - (n & 31)) & 31));
}

inline constexpr std::uint64_t rotl64(std::uint64_t x, unsigned n) {
    return (x << (n & 63)) | (x >> ((64 - (n & 63)) & 63));
}

inline constexpr std::uint64_t rotr64(std::uint64_t x, unsigned n) {
    return (x >> (n & 63)) | (x << ((64 - (n & 63)) & 63));
}

inline std::uint32_t load_le32(const unsigned char* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}

inline std::uint32_t load_be32(const unsigned char* p) {
    return (static_cast<std::uint32_t>(p[0]) << 24) |
           (static_cast<std::uint32_t>(p[1]) << 16) |
           (static_cast<std::uint32_t>(p[2]) << 8) |
           static_cast<std::uint32_t>(p[3]);
}

inline std::uint64_t load_le64(const unsigned char* p) {
    return static_cast<std::uint64_t>(load_le32(p)) |
           (static_cast<std::uint64_t>(load_le32(p + 4)) << 32);
}

inline std::uint64_t load_be64(const unsigned char* p) {
    return (static_cast<std::uint64_t>(load_be32(p)) << 32) |
           static_cast<std::uint64_t>(load_be32(p + 4));
}

inline void store_le32(unsigned char* p, std::uint32_t v) {
    p[0] = static_cast<unsigned char>(v & 0xFF);
    p[1] = static_cast<unsigned char>((v >> 8) & 0xFF);
    p[2] = static_cast<unsigned char>((v >> 16) & 0xFF);
    p[3] = static_cast<unsigned char>((v >> 24) & 0xFF);
}

inline void store_be32(unsigned char* p, std::uint32_t v) {
    p[0] = static_cast<unsigned char>((v >> 24) & 0xFF);
    p[1] = static_cast<unsigned char>((v >> 16) & 0xFF);
    p[2] = static_cast<unsigned char>((v >> 8) & 0xFF);
    p[3] = static_cast<unsigned char>(v & 0xFF);
}

inline void store_le64(unsigned char* p, std::uint64_t v) {
    store_le32(p, static_cast<std::uint32_t>(v & 0xFFFFFFFFULL));
    store_le32(p + 4, static_cast<std::uint32_t>(v >> 32));
}

inline void store_be64(unsigned char* p, std::uint64_t v) {
    store_be32(p, static_cast<std::uint32_t>(v >> 32));
    store_be32(p + 4, static_cast<std::uint32_t>(v & 0xFFFFFFFFULL));
}

inline std::string bytes_to_hex(const unsigned char* data, std::size_t len, bool uppercase = false) {
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string out;
    out.reserve(len * 2);
    for (std::size_t i = 0; i < len; ++i) {
        out += digits[data[i] >> 4];
        out += digits[data[i] & 0x0F];
    }
    return out;
}

inline std::string bytes_to_hex(const std::vector<unsigned char>& data, bool uppercase = false) {
    return bytes_to_hex(data.data(), data.size(), uppercase);
}

inline std::string u32_to_hex(std::uint32_t v, bool uppercase = false) {
    unsigned char b[4];
    store_be32(b, v);
    return bytes_to_hex(b, 4, uppercase);
}

inline std::string u64_to_hex(std::uint64_t v, bool uppercase = false) {
    unsigned char b[8];
    store_be64(b, v);
    return bytes_to_hex(b, 8, uppercase);
}

// Encodes ASCII/UTF-8 string to UTF-16LE bytes (used by NTLM and MSSQL hashes).
// Malformed, truncated, overlong, surrogate, or out-of-range sequences are replaced with U+FFFD.
inline std::string to_utf16le_bytes(const std::string& value) {
    static constexpr std::uint32_t min_value[] = {0x0, 0x80, 0x800, 0x10000};

    std::string out;
    std::size_t i = 0;
    while (i < value.size()) {
        unsigned char lead = static_cast<unsigned char>(value[i]);
        std::uint32_t cp = 0;
        std::size_t extra = 0;
        bool valid = true;

        if (lead < 0x80)                { cp = lead;        extra = 0; }
        else if ((lead & 0xE0) == 0xC0) { cp = lead & 0x1F; extra = 1; }
        else if ((lead & 0xF0) == 0xE0) { cp = lead & 0x0F; extra = 2; }
        else if ((lead & 0xF8) == 0xF0) { cp = lead & 0x07; extra = 3; }
        else { valid = false; }

        if (!valid || i + extra >= value.size()) {
            cp = 0xFFFD;
            ++i;
        } else {
            for (std::size_t j = 1; j <= extra; ++j) {
                unsigned char next = static_cast<unsigned char>(value[i + j]);
                if ((next & 0xC0) != 0x80) {
                    valid = false;
                    break;
                }
                cp = (cp << 6) | (next & 0x3F);
            }

            if (!valid || cp < min_value[extra] || cp > 0x10FFFF ||
                (cp >= 0xD800 && cp <= 0xDFFF)) {
                cp = 0xFFFD;
                ++i;
            } else {
                i += extra + 1;
            }
        }

        if (cp >= 0x10000) {
            cp -= 0x10000;
            std::uint16_t hi = static_cast<std::uint16_t>(0xD800 + (cp >> 10));
            std::uint16_t lo = static_cast<std::uint16_t>(0xDC00 + (cp & 0x3FF));
            out += static_cast<char>(hi & 0xFF);
            out += static_cast<char>((hi >> 8) & 0xFF);
            out += static_cast<char>(lo & 0xFF);
            out += static_cast<char>((lo >> 8) & 0xFF);
        } else {
            out += static_cast<char>(cp & 0xFF);
            out += static_cast<char>((cp >> 8) & 0xFF);
        }
    }
    return out;
}

// Generic HMAC construction over any raw hash function with given block_size.
template <typename RawHashFn>
std::vector<unsigned char> hmac_raw(RawHashFn hash_fn, std::size_t block_size,
                                    const unsigned char* key, std::size_t key_len,
                                    const unsigned char* msg, std::size_t msg_len) {
    std::vector<unsigned char> k_block(block_size, 0);
    if (key_len > block_size) {
        std::vector<unsigned char> kh = hash_fn(key, key_len);
        std::memcpy(k_block.data(), kh.data(), kh.size());
    } else if (key_len > 0) {
        std::memcpy(k_block.data(), key, key_len);
    }

    std::vector<unsigned char> ipad(block_size);
    std::vector<unsigned char> opad(block_size);
    for (std::size_t i = 0; i < block_size; ++i) {
        ipad[i] = static_cast<unsigned char>(k_block[i] ^ 0x36);
        opad[i] = static_cast<unsigned char>(k_block[i] ^ 0x5C);
    }

    ipad.insert(ipad.end(), msg, msg + msg_len);
    std::vector<unsigned char> inner = hash_fn(ipad.data(), ipad.size());
    opad.insert(opad.end(), inner.begin(), inner.end());
    return hash_fn(opad.data(), opad.size());
}

// Generic PBKDF2-HMAC derivation (RFC 2898).
template <typename RawHashFn>
std::vector<unsigned char> pbkdf2_raw(RawHashFn hash_fn, std::size_t block_size, std::size_t hash_len,
                                      const std::string& password, const std::string& salt,
                                      std::uint32_t iterations, std::size_t dk_len) {
    std::vector<unsigned char> derived;
    derived.reserve(dk_len);

    std::uint32_t block_index = 1;
    while (derived.size() < dk_len) {
        std::vector<unsigned char> salt_block(salt.begin(), salt.end());
        unsigned char idx_be[4];
        store_be32(idx_be, block_index++);
        salt_block.insert(salt_block.end(), idx_be, idx_be + 4);

        std::vector<unsigned char> u = hmac_raw(
            hash_fn, block_size,
            reinterpret_cast<const unsigned char*>(password.data()), password.size(),
            salt_block.data(), salt_block.size());
        std::vector<unsigned char> t = u;

        for (std::uint32_t iter = 1; iter < iterations; ++iter) {
            u = hmac_raw(
                hash_fn, block_size,
                reinterpret_cast<const unsigned char*>(password.data()), password.size(),
                u.data(), u.size());
            for (std::size_t j = 0; j < hash_len; ++j) {
                t[j] ^= u[j];
            }
        }

        std::size_t to_copy = std::min(hash_len, dk_len - derived.size());
        derived.insert(derived.end(), t.begin(), t.begin() + static_cast<std::ptrdiff_t>(to_copy));
    }

    return derived;
}

std::vector<unsigned char> md4_raw(const unsigned char* data, std::size_t len);
std::vector<unsigned char> md5_raw(const unsigned char* data, std::size_t len);
std::vector<unsigned char> sha1_raw(const unsigned char* data, std::size_t len);
std::vector<unsigned char> sha256_raw(const unsigned char* data, std::size_t len);
std::vector<unsigned char> sha512_raw(const unsigned char* data, std::size_t len);

// Keccak-f[1600] sponge used by Keccak-256/512, SHA3-224/256/384/512, and SHAKE128/256.
inline void keccak_f1600(std::uint64_t state[25]) {
    static constexpr std::uint64_t RC[24] = {
        0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
        0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
        0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
        0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
        0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
        0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
    };
    static constexpr int PILN[24] = {
        10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1
    };
    static constexpr unsigned ROTC[24] = {
        1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
    };

    for (int round = 0; round < 24; ++round) {
        std::uint64_t bc[5];
        for (int i = 0; i < 5; ++i) {
            bc[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }
        for (int i = 0; i < 5; ++i) {
            std::uint64_t t = bc[(i + 4) % 5] ^ rotl64(bc[(i + 1) % 5], 1);
            for (int j = 0; j < 25; j += 5) {
                state[j + i] ^= t;
            }
        }

        std::uint64_t t = state[1];
        for (int i = 0; i < 24; ++i) {
            int j = PILN[i];
            bc[0] = state[j];
            state[j] = rotl64(t, ROTC[i]);
            t = bc[0];
        }

        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; ++i) {
                bc[i] = state[j + i];
            }
            for (int i = 0; i < 5; ++i) {
                state[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }

        state[0] ^= RC[round];
    }
}

inline std::string keccak_sponge_hex(const std::string& value, std::size_t rate,
                                     unsigned char suffix, std::size_t out_len) {
    std::uint64_t state[25] = {0};
    unsigned char* sbytes = reinterpret_cast<unsigned char*>(state);

    std::size_t offset = 0;
    while (offset + rate <= value.size()) {
        for (std::size_t i = 0; i < rate / 8; ++i) {
            state[i] ^= load_le64(reinterpret_cast<const unsigned char*>(value.data() + offset + i * 8));
        }
        keccak_f1600(state);
        offset += rate;
    }

    unsigned char block[200] = {0};
    std::size_t rem = value.size() - offset;
    if (rem > 0) {
        std::memcpy(block, value.data() + offset, rem);
    }
    block[rem] = suffix;
    block[rate - 1] |= 0x80;

    for (std::size_t i = 0; i < rate / 8; ++i) {
        state[i] ^= load_le64(block + i * 8);
    }
    keccak_f1600(state);

    std::vector<unsigned char> out;
    out.reserve(out_len);
    while (out.size() < out_len) {
        std::size_t take = std::min(rate, out_len - out.size());
        for (std::size_t i = 0; i < take; ++i) {
            std::uint64_t lane = state[i / 8];
            out.push_back(static_cast<unsigned char>((lane >> ((i % 8) * 8)) & 0xFF));
        }
        if (out.size() < out_len) {
            keccak_f1600(state);
        }
    }
    (void)sbytes;
    return bytes_to_hex(out);
}

} // namespace hashing_utils

#endif
