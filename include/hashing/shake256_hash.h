#ifndef SHAKE256_HASH_H
#define SHAKE256_HASH_H

#include <cstddef>
#include <string>

std::string shake256_hash(const std::string& value);
std::string shake256_hash(const std::string& value, std::size_t out_bytes);

#endif // SHAKE256_HASH_H

