#ifndef XOR_ENCODE_H
#define XOR_ENCODE_H

#include <string>

#ifndef DEFAULT_XOR_KEY_DEFINED
#define DEFAULT_XOR_KEY_DEFINED
constexpr unsigned char DEFAULT_XOR_KEY = 0x5A;
#endif

std::string xor_encode(const std::string& value);
std::string xor_encode(const std::string& value, const std::string& key);

#endif

