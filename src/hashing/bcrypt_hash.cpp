#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "bcrypt_hash.h"
#include "hashing_utils.h"

namespace {

static constexpr char BCRYPT_B64[] =
    "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

int bcrypt_b64_val(char c) {
    if (c == '.') return 0;
    if (c == '/') return 1;
    if (c >= 'A' && c <= 'Z') return 2 + (c - 'A');
    if (c >= 'a' && c <= 'z') return 28 + (c - 'a');
    if (c >= '0' && c <= '9') return 54 + (c - '0');
    return 0;
}

std::string bcrypt_b64_encode(const std::uint8_t* data, std::size_t len) {
    std::string out;
    std::size_t i = 0;
    while (i < len) {
        std::uint32_t c1 = data[i++];
        out += BCRYPT_B64[(c1 >> 2) & 0x3FU];
        c1 = (c1 & 0x03U) << 4;
        if (i >= len) {
            out += BCRYPT_B64[c1 & 0x3FU];
            break;
        }
        std::uint32_t c2 = data[i++];
        c1 |= (c2 >> 4) & 0x0FU;
        out += BCRYPT_B64[c1 & 0x3FU];
        c1 = (c2 & 0x0FU) << 2;
        if (i >= len) {
            out += BCRYPT_B64[c1 & 0x3FU];
            break;
        }
        std::uint32_t c3 = data[i++];
        c1 |= (c3 >> 6) & 0x03U;
        out += BCRYPT_B64[c1 & 0x3FU];
        out += BCRYPT_B64[c3 & 0x3FU];
    }
    return out;
}

void bcrypt_b64_decode_salt(const std::string& s, std::uint8_t out[16]) {
    std::size_t idx = 0;
    std::size_t pos = 0;
    while (idx < 16) {
        int c1 = (pos < s.size()) ? bcrypt_b64_val(s[pos++]) : 0;
        int c2 = (pos < s.size()) ? bcrypt_b64_val(s[pos++]) : 0;
        out[idx++] = static_cast<std::uint8_t>(((c1 & 0x3F) << 2) | ((c2 >> 4) & 0x03));
        if (idx >= 16) break;

        int c3 = (pos < s.size()) ? bcrypt_b64_val(s[pos++]) : 0;
        out[idx++] = static_cast<std::uint8_t>(((c2 & 0x0F) << 4) | ((c3 >> 2) & 0x0F));
        if (idx >= 16) break;

        int c4 = (pos < s.size()) ? bcrypt_b64_val(s[pos++]) : 0;
        out[idx++] = static_cast<std::uint8_t>(((c3 & 0x03) << 6) | (c4 & 0x3F));
    }
}

// Compute the first 1042 32-bit fractional words of pi (pi - 3) for Blowfish P and S
struct BlowfishInitTables {
    std::uint32_t P[18];
    std::uint32_t S[4][256];

    static constexpr std::size_t WORDS = 1048;
    using BigFixed = std::array<std::uint32_t, WORDS>;

    static void div_small(BigFixed& a, std::uint32_t d) {
        std::uint64_t rem = 0;
        for (std::size_t i = 0; i < WORDS; ++i) {
            std::uint64_t cur = (rem << 32) | a[i];
            a[i] = static_cast<std::uint32_t>(cur / d);
            rem = cur % d;
        }
    }

    static bool is_zero(const BigFixed& a) {
        for (std::uint32_t w : a) {
            if (w != 0) return false;
        }
        return true;
    }

    static void add_fixed(BigFixed& a, const BigFixed& b) {
        std::uint64_t carry = 0;
        for (std::size_t i = WORDS; i-- > 0;) {
            std::uint64_t sum = static_cast<std::uint64_t>(a[i]) + b[i] + carry;
            a[i] = static_cast<std::uint32_t>(sum);
            carry = sum >> 32;
        }
    }

    static void sub_fixed(BigFixed& a, const BigFixed& b) {
        std::int64_t borrow = 0;
        for (std::size_t i = WORDS; i-- > 0;) {
            std::int64_t diff = static_cast<std::int64_t>(a[i]) - b[i] - borrow;
            if (diff < 0) {
                diff += (static_cast<std::int64_t>(1) << 32);
                borrow = 1;
            } else {
                borrow = 0;
            }
            a[i] = static_cast<std::uint32_t>(diff);
        }
    }

    static BigFixed arctan_scaled(std::uint32_t scale, std::uint32_t x) {
        BigFixed power{};
        power[0] = scale;
        div_small(power, x);
        BigFixed sum = power;
        std::uint32_t x2 = x * x;

        for (std::uint32_t k = 1;; ++k) {
            div_small(power, x2);
            if (is_zero(power)) break;
            BigFixed term = power;
            div_small(term, 2U * k + 1U);
            if ((k & 1U) != 0) {
                sub_fixed(sum, term);
            } else {
                add_fixed(sum, term);
            }
        }
        return sum;
    }

    BlowfishInitTables() : P{}, S{} {
        BigFixed pi = arctan_scaled(16, 5);
        BigFixed t2 = arctan_scaled(4, 239);
        sub_fixed(pi, t2);

        for (int i = 0; i < 18; ++i) {
            P[i] = pi[1 + i];
        }
        for (int s = 0; s < 4; ++s) {
            for (int i = 0; i < 256; ++i) {
                S[s][i] = pi[19 + s * 256 + i];
            }
        }
    }
};

const BlowfishInitTables& get_bf_init() {
    static const BlowfishInitTables init;
    return init;
}

struct BlowfishState {
    std::uint32_t P[18];
    std::uint32_t S[4][256];

    BlowfishState() {
        const auto& init = get_bf_init();
        for (int i = 0; i < 18; ++i) P[i] = init.P[i];
        for (int s = 0; s < 4; ++s) {
            for (int i = 0; i < 256; ++i) S[s][i] = init.S[s][i];
        }
    }

    inline std::uint32_t F(std::uint32_t x) const {
        return ((S[0][(x >> 24) & 0xFFU] + S[1][(x >> 16) & 0xFFU]) ^
                 S[2][(x >>  8) & 0xFFU]) + S[3][x & 0xFFU];
    }

    inline void encipher(std::uint32_t& xl, std::uint32_t& xr) const {
        std::uint32_t l = xl;
        std::uint32_t r = xr;
        for (int i = 0; i < 16; i += 2) {
            l ^= P[i];
            r ^= F(l);
            r ^= P[i + 1];
            l ^= F(r);
        }
        xl = r ^ P[17];
        xr = l ^ P[16];
    }

    static std::uint32_t next_word(const std::uint8_t* data, std::size_t len, std::size_t& pos) {
        std::uint32_t w = 0;
        for (int k = 0; k < 4; ++k) {
            w = (w << 8) | data[pos];
            pos = (pos + 1) % len;
        }
        return w;
    }

    void expand_state(const std::uint8_t* data, std::size_t data_len,
                      const std::uint8_t* key, std::size_t key_len) {
        std::size_t kpos = 0;
        for (int i = 0; i < 18; ++i) {
            P[i] ^= next_word(key, key_len, kpos);
        }
        std::uint32_t l = 0, r = 0;
        std::size_t dpos = 0;
        for (int i = 0; i < 18; i += 2) {
            l ^= next_word(data, data_len, dpos);
            r ^= next_word(data, data_len, dpos);
            encipher(l, r);
            P[i] = l;
            P[i + 1] = r;
        }
        for (int s = 0; s < 4; ++s) {
            for (int i = 0; i < 256; i += 2) {
                l ^= next_word(data, data_len, dpos);
                r ^= next_word(data, data_len, dpos);
                encipher(l, r);
                S[s][i] = l;
                S[s][i + 1] = r;
            }
        }
    }

    void expand_0state(const std::uint8_t* key, std::size_t key_len) {
        std::size_t kpos = 0;
        for (int i = 0; i < 18; ++i) {
            P[i] ^= next_word(key, key_len, kpos);
        }
        std::uint32_t l = 0, r = 0;
        for (int i = 0; i < 18; i += 2) {
            encipher(l, r);
            P[i] = l;
            P[i + 1] = r;
        }
        for (int s = 0; s < 4; ++s) {
            for (int i = 0; i < 256; i += 2) {
                encipher(l, r);
                S[s][i] = l;
                S[s][i + 1] = r;
            }
        }
    }
};

} // namespace

std::string bcrypt_hash(const std::string& value, unsigned int cost, const std::string& salt_b64) {
    if (cost < 4) cost = 4;
    if (cost > 31) cost = 31;

    std::string clean_salt = salt_b64;
    if (clean_salt.size() >= 29 && clean_salt[0] == '$' && clean_salt[1] == '2' &&
        clean_salt[3] == '$' && clean_salt[6] == '$') {
        clean_salt = clean_salt.substr(7, 22);
    }

    std::uint8_t salt_bytes[16];
    bcrypt_b64_decode_salt(clean_salt, salt_bytes);
    std::string canonical_salt = bcrypt_b64_encode(salt_bytes, 16);

    // $2b$ truncates passwords at 72 bytes and appends a NUL terminator byte
    std::size_t pass_len = (value.size() > 72) ? 72 : value.size();
    std::vector<std::uint8_t> key_bytes(pass_len + 1);
    for (std::size_t i = 0; i < pass_len; ++i) {
        key_bytes[i] = static_cast<std::uint8_t>(value[i]);
    }
    key_bytes[pass_len] = 0;

    BlowfishState state;
    state.expand_state(salt_bytes, 16, key_bytes.data(), key_bytes.size());

    std::uint64_t rounds = 1ULL << cost;
    for (std::uint64_t r = 0; r < rounds; ++r) {
        state.expand_0state(key_bytes.data(), key_bytes.size());
        state.expand_0state(salt_bytes, 16);
    }

    static constexpr char magic_str[25] = "OrpheanBeholderScryDoubt";
    std::uint32_t ctext[6];
    for (int i = 0; i < 6; ++i) {
        ctext[i] = hashing_utils::load_be32(
            reinterpret_cast<const unsigned char*>(magic_str + i * 4));
    }

    for (int i = 0; i < 6; i += 2) {
        for (int r = 0; r < 64; ++r) {
            state.encipher(ctext[i], ctext[i + 1]);
        }
    }

    std::uint8_t out_bytes[24];
    for (int i = 0; i < 6; ++i) {
        hashing_utils::store_be32(out_bytes + i * 4, ctext[i]);
    }

    std::string prefix = "$2b$";
    prefix += static_cast<char>('0' + (cost / 10));
    prefix += static_cast<char>('0' + (cost % 10));
    prefix += '$';

    return prefix + canonical_salt + bcrypt_b64_encode(out_bytes, 23);
}

std::string bcrypt_hash(const std::string& value) {
    // Default cost 5 and deterministic 16-byte salt ("saltsaltsaltsaltsalt..")
    return bcrypt_hash(value, 5, "saltsaltsaltsaltsalt..");
}
