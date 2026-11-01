#ifndef BCRYPT_HASH_H
#define BCRYPT_HASH_H

#include <string>

std::string bcrypt_hash(const std::string& value);
std::string bcrypt_hash(const std::string& value, unsigned int cost, const std::string& salt_b64);

#endif // BCRYPT_HASH_H

