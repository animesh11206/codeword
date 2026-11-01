#include <cstdint>
#include <string>
#include "octal_escape_decode.h"

// Decodes C-style octal escapes ("\110\151" -> "Hi").
std::string octal_escape_decode(const std::string& value) {
    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size() && value[i + 1] >= '0' && value[i + 1] <= '7') {
            std::uint32_t byte = 0;
            std::size_t count = 0;

            while (count < 3 && i + 1 < value.size() && value[i + 1] >= '0' && value[i + 1] <= '7') {
                byte = (byte << 3) | static_cast<std::uint32_t>(value[i + 1] - '0');
                ++i;
                ++count;
            }

            decoded += static_cast<char>(byte & 0xFF);
        }
        else {
            decoded += value[i];
        }
    }

    return decoded;
}

