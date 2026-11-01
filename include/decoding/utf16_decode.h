#ifndef UTF16_DECODE_H
#define UTF16_DECODE_H

#include <string>

std::string utf16_decode(const std::string& value);   // auto-detects BOM, defaults to little-endian
std::string utf16be_decode(const std::string& value); // big-endian, no BOM
std::string utf16le_decode(const std::string& value); // little-endian, no BOM

#endif

