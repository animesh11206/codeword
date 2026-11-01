#ifndef DECODING_FUNCTIONS_H
#define DECODING_FUNCTIONS_H

#include <string>

#include "decoding/ascii_decode.h"
#include "decoding/base32_decode.h"
#include "decoding/base32hex_decode.h"
#include "decoding/base58_decode.h"
#include "decoding/base62_decode.h"
#include "decoding/base64_decode.h"
#include "decoding/base64url_decode.h"
#include "decoding/base85_decode.h"
#include "decoding/binary_decode.h"
#include "decoding/decimal_decode.h"
#include "decoding/double_url_decode.h"
#include "decoding/hex_decode.h"
#include "decoding/hex_escape_decode.h"
#include "decoding/html_entity_dec_decode.h"
#include "decoding/html_entity_hex_decode.h"
#include "decoding/html_entity_named_decode.h"
#include "decoding/octal_decode.h"
#include "decoding/octal_escape_decode.h"
#include "decoding/punycode_decode.h"
#include "decoding/quoted_printable_decode.h"
#include "decoding/rot13_decode.h"
#include "decoding/rot47_decode.h"
#include "decoding/unicode_codepoint_decode.h"
#include "decoding/unicode_escape_decode.h"
#include "decoding/unicode_iis_decode.h"
#include "decoding/url_decode.h"
#include "decoding/url_all_decode.h"
#include "decoding/utf8_decode.h"
#include "decoding/utf16_decode.h"
#include "decoding/utf32_decode.h"
#include "decoding/uuencode_decode.h"
#include "decoding/xor_decode.h"

std::string decode(const std::string& flag, const std::string& value);

#endif // DECODING_FUNCTIONS_H
