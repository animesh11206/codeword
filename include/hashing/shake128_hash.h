#ifndef SHAKE128_HASH_H
#define SHAKE128_HASH_H

#include <cstddef>
#include <string>

std::string shake128_hash(const std::string& value);
std::string shake128_hash(const std::string& value, std::size_t out_bytes);

#endif // SHAKE128_HASH_H

