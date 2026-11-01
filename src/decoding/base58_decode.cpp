#include <string>
#include "base58_decode.h"
#include "decoding_utils.h"

std::string base58_decode(const std::string& value) {
    static constexpr char alphabet[] =
        "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    return decoding_utils::decode_big_base(value, alphabet, 58);
}

