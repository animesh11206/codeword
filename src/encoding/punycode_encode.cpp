#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include "punycode_encode.h"
#include "encoding_utils.h"

namespace {

// RFC 3492 parameters
constexpr std::uint32_t BASE = 36;
constexpr std::uint32_t T_MIN = 1;
constexpr std::uint32_t T_MAX = 26;
constexpr std::uint32_t SKEW = 38;
constexpr std::uint32_t DAMP = 700;
constexpr std::uint32_t INITIAL_BIAS = 72;
constexpr std::uint32_t INITIAL_N = 128;

char encode_digit(std::uint32_t digit) {
    return digit < 26 ? static_cast<char>('a' + digit) : static_cast<char>('0' + (digit - 26));
}

std::uint32_t adapt(std::uint32_t delta, std::uint32_t num_points, bool first_time) {
    delta = first_time ? delta / DAMP : delta / 2;
    delta += delta / num_points;

    std::uint32_t k = 0;
    while (delta > ((BASE - T_MIN) * T_MAX) / 2) {
        delta /= BASE - T_MIN;
        k += BASE;
    }

    return k + (BASE - T_MIN + 1) * delta / (delta + SKEW);
}

// Encodes one label with the RFC 3492 algorithm (no "xn--" prefix).
std::string encode_label(const std::vector<std::uint32_t>& input) {
    std::string output;

    for (std::uint32_t code_point : input) {
        if (code_point < 0x80) {
            output += static_cast<char>(code_point);
        }
    }

    const std::uint32_t basic_count = static_cast<std::uint32_t>(output.size());
    std::uint32_t handled = basic_count;

    if (basic_count > 0) {
        output += '-';
    }

    std::uint32_t n = INITIAL_N;
    std::uint32_t delta = 0;
    std::uint32_t bias = INITIAL_BIAS;

    while (handled < input.size()) {
        std::uint32_t m = std::numeric_limits<std::uint32_t>::max();
        for (std::uint32_t code_point : input) {
            if (code_point >= n && code_point < m) {
                m = code_point;
            }
        }

        delta += (m - n) * (handled + 1);
        n = m;

        for (std::uint32_t code_point : input) {
            if (code_point < n) {
                ++delta;
            }

            if (code_point == n) {
                std::uint32_t q = delta;

                for (std::uint32_t k = BASE;; k += BASE) {
                    std::uint32_t t = k <= bias ? T_MIN : (k >= bias + T_MAX ? T_MAX : k - bias);
                    if (q < t) {
                        break;
                    }
                    output += encode_digit(t + (q - t) % (BASE - t));
                    q = (q - t) / (BASE - t);
                }

                output += encode_digit(q);
                bias = adapt(delta, handled + 1, handled == basic_count);
                delta = 0;
                ++handled;
            }
        }

        ++delta;
        ++n;
    }

    return output;
}

} // namespace

// IDNA-style punycode: each dot-separated label containing non-ASCII characters
// becomes "xn--" + RFC 3492 punycode; pure-ASCII labels are left unchanged.
// Example: "münchen.de" -> "xn--mnchen-3ya.de"
std::string punycode_encode(const std::string& value) {
    std::string encoded;
    std::vector<std::uint32_t> label;

    auto flush_label = [&]() {
        bool all_ascii = true;
        for (std::uint32_t code_point : label) {
            if (code_point >= 0x80) {
                all_ascii = false;
                break;
            }
        }

        if (all_ascii) {
            for (std::uint32_t code_point : label) {
                encoded += static_cast<char>(code_point);
            }
        }
        else {
            encoded += "xn--";
            encoded += encode_label(label);
        }

        label.clear();
    };

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        if (code_point == '.') {
            flush_label();
            encoded += '.';
        }
        else {
            label.push_back(code_point);
        }
    }

    flush_label();
    return encoded;
}

