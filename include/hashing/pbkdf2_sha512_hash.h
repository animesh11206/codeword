#ifndef PBKDF2_SHA512_HASH_H
#define PBKDF2_SHA512_HASH_H

#include <cstddef>
#include <cstdint>
#include <string>

std::string pbkdf2_sha512_hash(const std::string& value);
std::string pbkdf2_sha512_hash(const std::string& value, const std::string& salt,
                               std::uint32_t iterations, std::size_t dk_len);

#endif // PBKDF2_SHA512_HASH_H

