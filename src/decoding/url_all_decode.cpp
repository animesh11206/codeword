#include <string>
#include "url_all_decode.h"
#include "url_decode.h"

// Decodes fully percent-encoded strings ("%48%69" -> "Hi").
std::string url_all_decode(const std::string& value) {
    return url_decode(value);
}

