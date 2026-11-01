#include <string>
#include "base62_decode.h"
#include "decoding_utils.h"

std::string base62_decode(const std::string& value) {
    static constexpr char alphabet[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    return decoding_utils::decode_big_base(value, alphabet, 62);
}

