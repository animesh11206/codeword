#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "sha512_crypt_hash.h"
#include "hashing_utils.h"

namespace {

constexpr char CRYPT_ITOA64[] =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(std::string& out, std::uint32_t v, int n) {
    while (--n >= 0) {
        out += CRYPT_ITOA64[v & 0x3F];
        v >>= 6;
    }
}

std::vector<unsigned char> sha512_str(const std::string& s) {
    return hashing_utils::sha512_raw(reinterpret_cast<const unsigned char*>(s.data()), s.size());
}

} // namespace

// Ulrich Drepper SHA-512 crypt ("$6$<salt>$<86-char hash>", 5000 rounds).
std::string sha512_crypt_hash(const std::string& value, const std::string& salt_in) {
    std::string clean_salt = salt_in;
    if (clean_salt.rfind("$6$", 0) == 0) {
        clean_salt.erase(0, 3);
    }
    std::size_t dollar = clean_salt.find('$');
    if (dollar != std::string::npos) {
        clean_salt.erase(dollar);
    }
    std::string salt = clean_salt.substr(0, std::min<std::size_t>(16, clean_salt.size()));

    auto alt_result = sha512_str(value + salt + value);

    std::string ctx = value + salt;
    for (std::size_t cnt = value.size(); cnt > 0; cnt -= std::min<std::size_t>(64, cnt)) {
        ctx.append(reinterpret_cast<const char*>(alt_result.data()), std::min<std::size_t>(64, cnt));
    }

    for (std::size_t cnt = value.size(); cnt > 0; cnt >>= 1) {
        if ((cnt & 1) != 0) {
            ctx.append(reinterpret_cast<const char*>(alt_result.data()), 64);
        } else {
            ctx += value;
        }
    }

    alt_result = sha512_str(ctx);

    std::string dp_ctx;
    for (std::size_t i = 0; i < value.size(); ++i) {
        dp_ctx += value;
    }
    auto temp_p = sha512_str(dp_ctx);
    std::string p_bytes;
    for (std::size_t cnt = value.size(); cnt > 0; cnt -= std::min<std::size_t>(64, cnt)) {
        p_bytes.append(reinterpret_cast<const char*>(temp_p.data()), std::min<std::size_t>(64, cnt));
    }

    std::string ds_ctx;
    for (std::size_t i = 0; i < 16U + alt_result[0]; ++i) {
        ds_ctx += salt;
    }
    auto temp_s = sha512_str(ds_ctx);
    std::string s_bytes;
    for (std::size_t cnt = salt.size(); cnt > 0; cnt -= std::min<std::size_t>(64, cnt)) {
        s_bytes.append(reinterpret_cast<const char*>(temp_s.data()), std::min<std::size_t>(64, cnt));
    }

    for (int i = 0; i < 5000; ++i) {
        std::string c_in;
        if ((i & 1) != 0) {
            c_in += p_bytes;
        } else {
            c_in.append(reinterpret_cast<const char*>(alt_result.data()), 64);
        }
        if (i % 3 != 0) {
            c_in += s_bytes;
        }
        if (i % 7 != 0) {
            c_in += p_bytes;
        }
        if ((i & 1) != 0) {
            c_in.append(reinterpret_cast<const char*>(alt_result.data()), 64);
        } else {
            c_in += p_bytes;
        }
        alt_result = sha512_str(c_in);
    }

    auto b64_3 = [&](int i0, int i1, int i2) {
        return (static_cast<std::uint32_t>(alt_result[i0]) << 16) |
               (static_cast<std::uint32_t>(alt_result[i1]) << 8)  |
                static_cast<std::uint32_t>(alt_result[i2]);
    };

    std::string out = "$6$" + salt + "$";
    to64(out, b64_3(0, 21, 42), 4);
    to64(out, b64_3(22, 43, 1), 4);
    to64(out, b64_3(44, 2, 23), 4);
    to64(out, b64_3(3, 24, 45), 4);
    to64(out, b64_3(25, 46, 4), 4);
    to64(out, b64_3(47, 5, 26), 4);
    to64(out, b64_3(6, 27, 48), 4);
    to64(out, b64_3(28, 49, 7), 4);
    to64(out, b64_3(50, 8, 29), 4);
    to64(out, b64_3(9, 30, 51), 4);
    to64(out, b64_3(31, 52, 10), 4);
    to64(out, b64_3(53, 11, 32), 4);
    to64(out, b64_3(12, 33, 54), 4);
    to64(out, b64_3(34, 55, 13), 4);
    to64(out, b64_3(56, 14, 35), 4);
    to64(out, b64_3(15, 36, 57), 4);
    to64(out, b64_3(37, 58, 16), 4);
    to64(out, b64_3(59, 17, 38), 4);
    to64(out, b64_3(18, 39, 60), 4);
    to64(out, b64_3(40, 61, 19), 4);
    to64(out, b64_3(62, 20, 41), 4);
    to64(out, alt_result[63], 2);

    return out;
}

std::string sha512_crypt_hash(const std::string& value) {
    return sha512_crypt_hash(value, "saltsalt");
}

