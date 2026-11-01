#include <cstdint>
#include <string>
#include <vector>
#include "md5_hash.h"
#include "hashing_utils.h"

namespace hashing_utils {

std::vector<unsigned char> md5_raw(const unsigned char* data, std::size_t len) {
    static constexpr unsigned char S[64] = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };

    static constexpr std::uint32_t K[64] = {
        0xd76aa478U, 0xe8c7b756U, 0x242070dbU, 0xc1bdceeeU,
        0xf57c0fafU, 0x4787c62aU, 0xa8304613U, 0xfd469501U,
        0x698098d8U, 0x8b44f7afU, 0xffff5bb1U, 0x895cd7beU,
        0x6b901122U, 0xfd987193U, 0xa679438eU, 0x49b40821U,
        0xf61e2562U, 0xc040b340U, 0x265e5a51U, 0xe9b6c7aaU,
        0xd62f105dU, 0x02441453U, 0xd8a1e681U, 0xe7d3fbc8U,
        0x21e1cde6U, 0xc33707d6U, 0xf4d50d87U, 0x455a14edU,
        0xa9e3e905U, 0xfcefa3f8U, 0x676f02d9U, 0x8d2a4c8aU,
        0xfffa3942U, 0x8771f681U, 0x6d9d6122U, 0xfde5380cU,
        0xa4beea44U, 0x4bdecfa9U, 0xf6bb4b60U, 0xbebfbc70U,
        0x289b7ec6U, 0xeaa127faU, 0xd4ef3085U, 0x04881d05U,
        0xd9d4d039U, 0xe6db99e5U, 0x1fa27cf8U, 0xc4ac5665U,
        0xf4292244U, 0x432aff97U, 0xab9423a7U, 0xfc93a039U,
        0x655b59c3U, 0x8f0ccc92U, 0xffeff47dU, 0x85845dd1U,
        0x6fa87e4fU, 0xfe2ce6e0U, 0xa3014314U, 0x4e0811a1U,
        0xf7537e82U, 0xbd3af235U, 0x2ad7d2bbU, 0xeb86d391U
    };

    std::vector<unsigned char> msg(data, data + len);
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    std::uint64_t bit_len = static_cast<std::uint64_t>(len) * 8;
    unsigned char len_bytes[8];
    store_le64(len_bytes, bit_len);
    msg.insert(msg.end(), len_bytes, len_bytes + 8);

    std::uint32_t a0 = 0x67452301U;
    std::uint32_t b0 = 0xefcdab89U;
    std::uint32_t c0 = 0x98badcfeU;
    std::uint32_t d0 = 0x10325476U;

    for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
        std::uint32_t m[16];
        for (int i = 0; i < 16; ++i) {
            m[i] = load_le32(&msg[offset + i * 4]);
        }

        std::uint32_t a = a0, b = b0, c = c0, d = d0;

        for (int i = 0; i < 64; ++i) {
            std::uint32_t f = 0;
            int g = 0;
            if (i < 16) {
                f = (b & c) | (~b & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | (~d & c);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            } else {
                f = c ^ (b | ~d);
                g = (7 * i) % 16;
            }

            f = f + a + K[i] + m[g];
            a = d;
            d = c;
            c = b;
            b = b + rotl32(f, S[i]);
        }

        a0 += a; b0 += b; c0 += c; d0 += d;
    }

    std::vector<unsigned char> digest(16);
    store_le32(&digest[0], a0);
    store_le32(&digest[4], b0);
    store_le32(&digest[8], c0);
    store_le32(&digest[12], d0);
    return digest;
}

} // namespace hashing_utils

// RFC 1321 MD5 message digest (32 lowercase hex characters).
std::string md5_hash(const std::string& value) {
    auto digest = hashing_utils::md5_raw(reinterpret_cast<const unsigned char*>(value.data()), value.size());
    return hashing_utils::bytes_to_hex(digest);
}

