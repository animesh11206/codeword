#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "md5_crypt_hash.h"
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

} // namespace

// FreeBSD MD5-crypt ("$1$<salt>$<22-char hash>").
std::string md5_crypt_hash(const std::string& value, const std::string& salt_in) {
    std::string clean_salt = salt_in;
    if (clean_salt.rfind("$1$", 0) == 0) {
        clean_salt.erase(0, 3);
    }
    std::size_t dollar = clean_salt.find('$');
    if (dollar != std::string::npos) {
        clean_salt.erase(dollar);
    }
    std::string salt = clean_salt.substr(0, std::min<std::size_t>(8, clean_salt.size()));

    std::string alt_input = value + salt + value;
    auto alt_result = hashing_utils::md5_raw(
        reinterpret_cast<const unsigned char*>(alt_input.data()), alt_input.size());

    std::string ctx_input = value + "$1$" + salt;
    for (std::size_t cnt = value.size(); cnt > 0; cnt -= std::min<std::size_t>(16, cnt)) {
        std::size_t take = std::min<std::size_t>(16, cnt);
        ctx_input.append(reinterpret_cast<const char*>(alt_result.data()), take);
    }

    for (std::size_t cnt = value.size(); cnt > 0; cnt >>= 1) {
        if ((cnt & 1) != 0) {
            ctx_input += '\0';
        } else {
            ctx_input += value.empty() ? '\0' : value[0];
        }
    }

    auto final_result = hashing_utils::md5_raw(
        reinterpret_cast<const unsigned char*>(ctx_input.data()), ctx_input.size());

    for (int i = 0; i < 1000; ++i) {
        std::string r_in;
        if ((i & 1) != 0) {
            r_in += value;
        } else {
            r_in.append(reinterpret_cast<const char*>(final_result.data()), 16);
        }

        if (i % 3 != 0) {
            r_in += salt;
        }
        if (i % 7 != 0) {
            r_in += value;
        }

        if ((i & 1) != 0) {
            r_in.append(reinterpret_cast<const char*>(final_result.data()), 16);
        } else {
            r_in += value;
        }

        final_result = hashing_utils::md5_raw(
            reinterpret_cast<const unsigned char*>(r_in.data()), r_in.size());
    }

    std::string out = "$1$" + salt + "$";
    to64(out, (static_cast<std::uint32_t>(final_result[0]) << 16) |
              (static_cast<std::uint32_t>(final_result[6]) << 8)  | final_result[12], 4);
    to64(out, (static_cast<std::uint32_t>(final_result[1]) << 16) |
              (static_cast<std::uint32_t>(final_result[7]) << 8)  | final_result[13], 4);
    to64(out, (static_cast<std::uint32_t>(final_result[2]) << 16) |
              (static_cast<std::uint32_t>(final_result[8]) << 8)  | final_result[14], 4);
    to64(out, (static_cast<std::uint32_t>(final_result[3]) << 16) |
              (static_cast<std::uint32_t>(final_result[9]) << 8)  | final_result[15], 4);
    to64(out, (static_cast<std::uint32_t>(final_result[4]) << 16) |
              (static_cast<std::uint32_t>(final_result[10]) << 8) | final_result[5],  4);
    to64(out, final_result[11], 2);

    return out;
}

std::string md5_crypt_hash(const std::string& value) {
    return md5_crypt_hash(value, "saltsalt");
}

