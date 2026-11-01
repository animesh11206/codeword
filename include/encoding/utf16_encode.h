#ifndef UTF16_ENCODE_H
#define UTF16_ENCODE_H

#include <string>

std::string utf16_encode(const std::string& value);   // little-endian with BOM
std::string utf16be_encode(const std::string& value); // big-endian, no BOM
std::string utf16le_encode(const std::string& value); // little-endian, no BOM

#endif

