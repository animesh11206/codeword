#ifndef UTF32_ENCODE_H
#define UTF32_ENCODE_H

#include <string>

std::string utf32_encode(const std::string& value);   // little-endian with BOM
std::string utf32be_encode(const std::string& value); // big-endian, no BOM
std::string utf32le_encode(const std::string& value); // little-endian, no BOM

#endif

