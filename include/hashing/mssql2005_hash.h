#ifndef MSSQL2005_HASH_H
#define MSSQL2005_HASH_H

#include <cstdint>
#include <string>

constexpr std::uint32_t DEFAULT_MSSQL2005_SALT = 0x12345678U;

std::string mssql2005_hash(const std::string& value);
std::string mssql2005_hash(const std::string& value, std::uint32_t salt);

#endif // MSSQL2005_HASH_H

