#include <string>
#include "double_url_decode.h"
#include "url_decode.h"

// URL-decodes the string twice ("a%2520b" -> "a%20b" -> "a b").
std::string double_url_decode(const std::string& value) {
    return url_decode(url_decode(value));
}

