#include <string>
#include "base62_encode.h"
#include "encoding_utils.h"

std::string base62_encode(const std::string& value) {
    static constexpr char alphabet[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    return encoding_utils::encode_big_base(value, alphabet, 62);
}

