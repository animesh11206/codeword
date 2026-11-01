#include <cctype>
#include <cstdint>
#include <string>
#include "mssql2000_hash.h"
#include "hashing_utils.h"

// Microsoft SQL Server 2000 password hash:
// "0x0100" + HEX(salt) + HEX(SHA1(UTF16LE(password) + salt)) + HEX(SHA1(UTF16LE(UPPER(password)) + salt))
std::string mssql2000_hash(const std::string& value, std::uint32_t salt) {
    unsigned char salt_bytes[4];
    hashing_utils::store_be32(salt_bytes, salt);
    std::string salt_str(reinterpret_cast<const char*>(salt_bytes), 4);

    std::string upper_val = value;
    for (char& c : upper_val) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    std::string mixed_in = hashing_utils::to_utf16le_bytes(value) + salt_str;
    std::string upper_in = hashing_utils::to_utf16le_bytes(upper_val) + salt_str;

    auto mixed_sha = hashing_utils::sha1_raw(reinterpret_cast<const unsigned char*>(mixed_in.data()), mixed_in.size());
    auto upper_sha = hashing_utils::sha1_raw(reinterpret_cast<const unsigned char*>(upper_in.data()), upper_in.size());

    return "0x0100" +
           hashing_utils::bytes_to_hex(salt_bytes, 4, true) +
           hashing_utils::bytes_to_hex(mixed_sha, true) +
           hashing_utils::bytes_to_hex(upper_sha, true);
}

std::string mssql2000_hash(const std::string& value) {
    return mssql2000_hash(value, DEFAULT_MSSQL2000_SALT);
}

