#ifndef UTF32_DECODE_H
#define UTF32_DECODE_H

#include <string>

std::string utf32_decode(const std::string& value);   // auto-detects BOM, defaults to little-endian
std::string utf32be_decode(const std::string& value); // big-endian, no BOM
std::string utf32le_decode(const std::string& value); // little-endian, no BOM

#endif

