#include <cstdint>
#include <string>
#include "mssql2012_hash.h"
#include "hashing_utils.h"

// Microsoft SQL Server 2012+ password hash:
// "0x0200" + HEX(salt) + HEX(SHA512(UTF16LE(password) + salt))
std::string mssql2012_hash(const std::string& value, std::uint32_t salt) {
    unsigned char salt_bytes[4];
    hashing_utils::store_be32(salt_bytes, salt);
    std::string salt_str(reinterpret_cast<const char*>(salt_bytes), 4);

    std::string input = hashing_utils::to_utf16le_bytes(value) + salt_str;
    auto sha = hashing_utils::sha512_raw(reinterpret_cast<const unsigned char*>(input.data()), input.size());

    return "0x0200" +
           hashing_utils::bytes_to_hex(salt_bytes, 4, true) +
           hashing_utils::bytes_to_hex(sha, true);
}

std::string mssql2012_hash(const std::string& value) {
    return mssql2012_hash(value, DEFAULT_MSSQL2012_SALT);
}

