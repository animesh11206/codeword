#include <string>
#include "postgres_md5_hash.h"
#include "hashing_utils.h"

// PostgreSQL MD5 password hash: "md5" + MD5(password + username).
std::string postgres_md5_hash(const std::string& value, const std::string& username) {
    std::string combined = value + username;
    auto digest = hashing_utils::md5_raw(reinterpret_cast<const unsigned char*>(combined.data()), combined.size());
    return "md5" + hashing_utils::bytes_to_hex(digest);
}

std::string postgres_md5_hash(const std::string& value) {
    return postgres_md5_hash(value, "postgres");
}

