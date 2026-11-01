#include <algorithm>
#include <string>
#include "uuencode_decode.h"

// Decodes uuencoded data chunks (supporting both single-line concatenated chunks and multi-line input).
std::string uuencode_decode(const std::string& value) {
    auto decode_char = [](unsigned char c) -> unsigned {
        if (c == '`' || c <= 32) {
            return 0;
        }
        return static_cast<unsigned>((c - 32) & 0x3F);
    };

    std::string decoded;
    std::size_t pos = 0;

    while (pos < value.size()) {
        std::size_t line_end = value.find('\n', pos);
        if (line_end == std::string::npos) {
            line_end = value.size();
        }

        std::string line = value.substr(pos, line_end - pos);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        pos = line_end + 1;

        std::size_t idx = 0;
        while (idx < line.size()) {
            std::size_t chunk_bytes = decode_char(static_cast<unsigned char>(line[idx++]));
            if (chunk_bytes == 0) {
                break;
            }

            std::size_t groups = (chunk_bytes + 2) / 3;
            std::size_t emitted = 0;

            for (std::size_t g = 0; g < groups && idx < line.size(); ++g, idx += 4) {
                unsigned c0 = decode_char(static_cast<unsigned char>(line[idx]));
                unsigned c1 = idx + 1 < line.size() ? decode_char(static_cast<unsigned char>(line[idx + 1])) : 0;
                unsigned c2 = idx + 2 < line.size() ? decode_char(static_cast<unsigned char>(line[idx + 2])) : 0;
                unsigned c3 = idx + 3 < line.size() ? decode_char(static_cast<unsigned char>(line[idx + 3])) : 0;

                if (emitted < chunk_bytes) {
                    decoded += static_cast<char>((c0 << 2) | (c1 >> 4));
                    ++emitted;
                }
                if (emitted < chunk_bytes) {
                    decoded += static_cast<char>(((c1 & 0x0F) << 4) | (c2 >> 2));
                    ++emitted;
                }
                if (emitted < chunk_bytes) {
                    decoded += static_cast<char>(((c2 & 0x03) << 6) | c3);
                    ++emitted;
                }
            }
        }
    }

    return decoded;
}
