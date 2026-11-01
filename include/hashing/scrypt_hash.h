#ifndef SCRYPT_HASH_H
#define SCRYPT_HASH_H

#include <cstddef>
#include <cstdint>
#include <string>

std::string scrypt_hash(const std::string& value);
std::string scrypt_hash(const std::string& value, const std::string& salt,
                        std::uint32_t n, std::uint32_t r, std::uint32_t p,
                        std::size_t dk_len);

#endif // SCRYPT_HASH_H

