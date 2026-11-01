#ifndef MD5_CRYPT_HASH_H
#define MD5_CRYPT_HASH_H

#include <string>

std::string md5_crypt_hash(const std::string& value);
std::string md5_crypt_hash(const std::string& value, const std::string& salt_in);

#endif // MD5_CRYPT_HASH_H

