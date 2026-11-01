#include <string>
#include "double_url_encode.h"
#include "url_encode.h"

// URL-encodes the value twice: "a b" -> "a%20b" -> "a%2520b"
std::string double_url_encode(const std::string& value) {
    return url_encode(url_encode(value));
}

