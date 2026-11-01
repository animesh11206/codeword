#include <cstdint>
#include <string>
#include <vector>
#include "base85_decode.h"

// Decodes Ascii85 (Adobe/btoa style), supporting 'z' for four zero bytes and partial final groups.
std::string base85_decode(const std::string& value) {
    std::string decoded;
    std::vector<int> group;
    group.reserve(5);

    auto flush_group = [&]() {
        if (group.size() <= 1) {
            group.clear();
            return;
        }

        std::size_t count = group.size();
        while (group.size() < 5) {
            group.push_back(84); // pad with 'u' (84)
        }

        std::uint32_t tuple = 0;
        for (int digit : group) {
            tuple = tuple * 85 + static_cast<std::uint32_t>(digit);
        }

        for (std::size_t i = 0; i + 1 < count; ++i) {
            int shift = static_cast<int>((3 - i) * 8);
            decoded += static_cast<char>((tuple >> shift) & 0xFF);
        }

        group.clear();
    };

    for (unsigned char c : value) {
        if (c == 'z' && group.empty()) {
            decoded.append(4, '\0');
        }
        else if (c >= '!' && c <= 'u') {
            group.push_back(c - 33);
            if (group.size() == 5) {
                flush_group();
            }
        }
    }

    flush_group();
    return decoded;
}

