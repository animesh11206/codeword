#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "sha256_crypt_hash.h"
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

std::vector<unsigned char> sha256_str(const std::string& s) {
    return hashing_utils::sha256_raw(reinterpret_cast<const unsigned char*>(s.data()), s.size());
}

} // namespace

// Ulrich Drepper SHA-256 crypt ("$5$<salt>$<43-char hash>", 5000 rounds).
std::string sha256_crypt_hash(const std::string& value, const std::string& salt_in) {
    std::string clean_salt = salt_in;
    if (clean_salt.rfind("$5$", 0) == 0) {
        clean_salt.erase(0, 3);
    }
    std::size_t dollar = clean_salt.find('$');
    if (dollar != std::string::npos) {
        clean_salt.erase(dollar);
    }
    std::string salt = clean_salt.substr(0, std::min<std::size_t>(16, clean_salt.size()));

    auto alt_result = sha256_str(value + salt + value);

    std::string ctx = value + salt;
    for (std::size_t cnt = value.size(); cnt > 0; cnt -= std::min<std::size_t>(32, cnt)) {
        ctx.append(reinterpret_cast<const char*>(alt_result.data()), std::min<std::size_t>(32, cnt));
    }

    for (std::size_t cnt = value.size(); cnt > 0; cnt >>= 1) {
        if ((cnt & 1) != 0) {
            ctx.append(reinterpret_cast<const char*>(alt_result.data()), 32);
        } else {
            ctx += value;
        }
    }

    alt_result = sha256_str(ctx);

    std::string dp_ctx;
    for (std::size_t i = 0; i < value.size(); ++i) {
        dp_ctx += value;
    }
    auto temp_p = sha256_str(dp_ctx);
    std::string p_bytes;
    for (std::size_t cnt = value.size(); cnt > 0; cnt -= std::min<std::size_t>(32, cnt)) {
        p_bytes.append(reinterpret_cast<const char*>(temp_p.data()), std::min<std::size_t>(32, cnt));
    }

    std::string ds_ctx;
    for (std::size_t i = 0; i < 16U + alt_result[0]; ++i) {
        ds_ctx += salt;
    }
    auto temp_s = sha256_str(ds_ctx);
    std::string s_bytes;
    for (std::size_t cnt = salt.size(); cnt > 0; cnt -= std::min<std::size_t>(32, cnt)) {
        s_bytes.append(reinterpret_cast<const char*>(temp_s.data()), std::min<std::size_t>(32, cnt));
    }

    for (int i = 0; i < 5000; ++i) {
        std::string c_in;
        if ((i & 1) != 0) {
            c_in += p_bytes;
        } else {
            c_in.append(reinterpret_cast<const char*>(alt_result.data()), 32);
        }
        if (i % 3 != 0) {
            c_in += s_bytes;
        }
        if (i % 7 != 0) {
            c_in += p_bytes;
        }
        if ((i & 1) != 0) {
            c_in.append(reinterpret_cast<const char*>(alt_result.data()), 32);
        } else {
            c_in += p_bytes;
        }
        alt_result = sha256_str(c_in);
    }

    auto b64_3 = [&](int i0, int i1, int i2) {
        return (static_cast<std::uint32_t>(alt_result[i0]) << 16) |
               (static_cast<std::uint32_t>(alt_result[i1]) << 8)  |
                static_cast<std::uint32_t>(alt_result[i2]);
    };

    std::string out = "$5$" + salt + "$";
    to64(out, b64_3(0, 10, 20), 4);
    to64(out, b64_3(21, 1, 11), 4);
    to64(out, b64_3(12, 22, 2), 4);
    to64(out, b64_3(3, 13, 23), 4);
    to64(out, b64_3(24, 4, 14), 4);
    to64(out, b64_3(15, 25, 5), 4);
    to64(out, b64_3(6, 16, 26), 4);
    to64(out, b64_3(27, 7, 17), 4);
    to64(out, b64_3(18, 28, 8), 4);
    to64(out, b64_3(9, 19, 29), 4);
    to64(out, (static_cast<std::uint32_t>(alt_result[31]) << 8) | alt_result[30], 3);

    return out;
}

std::string sha256_crypt_hash(const std::string& value) {
    return sha256_crypt_hash(value, "saltsalt");
}

