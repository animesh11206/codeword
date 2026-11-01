#include <algorithm>
#include <cstdint>
#include <string>
#include "base85_encode.h"

// Ascii85 (Adobe/btoa style) without the <~ ~> delimiters.
std::string base85_encode(const std::string& value) {
    std::string encoded;
    std::size_t i = 0;

    while (i < value.size()) {
        std::size_t chunk = std::min<std::size_t>(4, value.size() - i);
        std::uint32_t tuple = 0;

        for (std::size_t j = 0; j < 4; ++j) {
            tuple <<= 8;
            if (j < chunk) {
                tuple |= static_cast<unsigned char>(value[i + j]);
            }
        }

        if (chunk == 4 && tuple == 0) {
            encoded += 'z'; // shorthand for four zero bytes
        }
        else {
            char block[5];
            for (int k = 4; k >= 0; --k) {
                block[k] = static_cast<char>(tuple % 85 + 33);
                tuple /= 85;
            }
            encoded.append(block, chunk + 1); // partial groups emit chunk + 1 chars
        }

        i += chunk;
    }

    return encoded;
}

