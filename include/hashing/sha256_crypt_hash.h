#ifndef SHA256_CRYPT_HASH_H
#define SHA256_CRYPT_HASH_H

#include <string>

std::string sha256_crypt_hash(const std::string& value);
std::string sha256_crypt_hash(const std::string& value, const std::string& salt_in);

#endif // SHA256_CRYPT_HASH_H

