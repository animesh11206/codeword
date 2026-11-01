#ifndef POSTGRES_MD5_HASH_H
#define POSTGRES_MD5_HASH_H

#include <string>

std::string postgres_md5_hash(const std::string& value);
std::string postgres_md5_hash(const std::string& value, const std::string& username);

#endif // POSTGRES_MD5_HASH_H

