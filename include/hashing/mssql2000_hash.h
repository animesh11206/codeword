#ifndef MSSQL2000_HASH_H
#define MSSQL2000_HASH_H

#include <cstdint>
#include <string>

constexpr std::uint32_t DEFAULT_MSSQL2000_SALT = 0x12345678U;

std::string mssql2000_hash(const std::string& value);
std::string mssql2000_hash(const std::string& value, std::uint32_t salt);

#endif // MSSQL2000_HASH_H

