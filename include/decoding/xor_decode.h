#ifndef XOR_DECODE_H
#define XOR_DECODE_H

#include <string>

#ifndef DEFAULT_XOR_KEY_DEFINED
#define DEFAULT_XOR_KEY_DEFINED
constexpr unsigned char DEFAULT_XOR_KEY = 0x5A;
#endif

std::string xor_decode(const std::string& value);
std::string xor_decode(const std::string& value, const std::string& key);

#endif

