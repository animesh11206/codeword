#include <string>
#include "mysql41_hash.h"
#include "hashing_utils.h"

// MySQL 4.1+ PASSWORD() hash: "*" + UPPERHEX(SHA1(SHA1(password))).
std::string mysql41_hash(const std::string& value) {
    auto stage1 = hashing_utils::sha1_raw(reinterpret_cast<const unsigned char*>(value.data()), value.size());
    auto stage2 = hashing_utils::sha1_raw(stage1.data(), stage1.size());
    return "*" + hashing_utils::bytes_to_hex(stage2, true);
}

