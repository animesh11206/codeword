#include <string>
#include "base58_encode.h"
#include "encoding_utils.h"

std::string base58_encode(const std::string& value) {
    // Bitcoin alphabet (no 0, O, I, l)
    static constexpr char alphabet[] =
        "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    return encoding_utils::encode_big_base(value, alphabet, 58);
}

