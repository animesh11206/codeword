#include <cstdint>
#include <string>
#include <vector>
#include "punycode_decode.h"
#include "decoding_utils.h"

namespace {

// RFC 3492 parameters
constexpr std::uint32_t BASE = 36;
constexpr std::uint32_t T_MIN = 1;
constexpr std::uint32_t T_MAX = 26;
constexpr std::uint32_t SKEW = 38;
constexpr std::uint32_t DAMP = 700;
constexpr std::uint32_t INITIAL_BIAS = 72;
constexpr std::uint32_t INITIAL_N = 128;

std::uint32_t decode_digit(char c) {
    if (c >= 'a' && c <= 'z') return static_cast<std::uint32_t>(c - 'a');
    if (c >= 'A' && c <= 'Z') return static_cast<std::uint32_t>(c - 'A');
    if (c >= '0' && c <= '9') return static_cast<std::uint32_t>(c - '0' + 26);
    return BASE;
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

// Decodes a single RFC 3492 punycode payload (after stripping "xn--").
std::string decode_label(const std::string& input) {
    std::vector<std::uint32_t> output;

    std::size_t delim = input.rfind('-');
    std::size_t pos = 0;

    if (delim != std::string::npos) {
        for (std::size_t j = 0; j < delim; ++j) {
            output.push_back(static_cast<unsigned char>(input[j]));
        }
        pos = delim + 1;
    }

    std::uint32_t n = INITIAL_N;
    std::uint32_t i = 0;
    std::uint32_t bias = INITIAL_BIAS;

    while (pos < input.size()) {
        std::uint32_t old_i = i;
        std::uint32_t w = 1;

        for (std::uint32_t k = BASE;; k += BASE) {
            if (pos >= input.size()) {
                break;
            }
            std::uint32_t digit = decode_digit(input[pos++]);
            if (digit >= BASE) {
                break;
            }

            i += digit * w;
            std::uint32_t t = k <= bias ? T_MIN : (k >= bias + T_MAX ? T_MAX : k - bias);
            if (digit < t) {
                break;
            }
            w *= (BASE - t);
        }

        std::uint32_t out_len = static_cast<std::uint32_t>(output.size() + 1);
        bias = adapt(i - old_i, out_len, old_i == 0);
        n += i / out_len;
        i %= out_len;

        output.insert(output.begin() + i, n);
        ++i;
    }

    std::string utf8;
    for (std::uint32_t cp : output) {
        decoding_utils::append_utf8(utf8, cp);
    }
    return utf8;
}

} // namespace

// Decodes IDNA punycode labels starting with "xn--" ("xn--mnchen-3ya.de" -> "münchen.de").
std::string punycode_decode(const std::string& value) {
    std::string decoded;
    std::size_t start = 0;

    while (start <= value.size()) {
        std::size_t dot = value.find('.', start);
        std::string label = (dot == std::string::npos)
            ? value.substr(start)
            : value.substr(start, dot - start);

        if (label.size() >= 4 &&
            (label[0] == 'x' || label[0] == 'X') &&
            (label[1] == 'n' || label[1] == 'N') &&
            label[2] == '-' && label[3] == '-') {
            decoded += decode_label(label.substr(4));
        }
        else {
            decoded += label;
        }

        if (dot == std::string::npos) {
            break;
        }
        decoded += '.';
        start = dot + 1;
    }

    return decoded;
}

