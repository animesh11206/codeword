#ifndef MSSQL2012_HASH_H
#define MSSQL2012_HASH_H

#include <cstdint>
#include <string>

constexpr std::uint32_t DEFAULT_MSSQL2012_SALT = 0x12345678U;

std::string mssql2012_hash(const std::string& value);
std::string mssql2012_hash(const std::string& value, std::uint32_t salt);

#endif // MSSQL2012_HASH_H

