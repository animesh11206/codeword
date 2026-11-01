#ifndef SHA512_CRYPT_HASH_H
#define SHA512_CRYPT_HASH_H

#include <string>

std::string sha512_crypt_hash(const std::string& value);
std::string sha512_crypt_hash(const std::string& value, const std::string& salt_in);

#endif // SHA512_CRYPT_HASH_H

