#ifndef ENCODING_FUNCTIONS_H
#define ENCODING_FUNCTIONS_H

#include <string>

#include "encoding/ascii_encode.h"
#include "encoding/base32_encode.h"
#include "encoding/base32hex_encode.h"
#include "encoding/base58_encode.h"
#include "encoding/base62_encode.h"
#include "encoding/base64_encode.h"
#include "encoding/base64url_encode.h"
#include "encoding/base85_encode.h"
#include "encoding/binary_encode.h"
#include "encoding/decimal_encode.h"
#include "encoding/double_url_encode.h"
#include "encoding/hex_encode.h"
#include "encoding/hex_escape_encode.h"
#include "encoding/html_entity_dec_encode.h"
#include "encoding/html_entity_hex_encode.h"
#include "encoding/html_entity_named_encode.h"
#include "encoding/octal_encode.h"
#include "encoding/octal_escape_encode.h"
#include "encoding/punycode_encode.h"
#include "encoding/quoted_printable_encode.h"
#include "encoding/rot13_encode.h"
#include "encoding/rot47_encode.h"
#include "encoding/unicode_codepoint_encode.h"
#include "encoding/unicode_escape_encode.h"
#include "encoding/unicode_iis_encode.h"
#include "encoding/url_encode.h"
#include "encoding/url_all_encode.h"
#include "encoding/utf8_encode.h"
#include "encoding/utf16_encode.h"
#include "encoding/utf32_encode.h"
#include "encoding/uuencode_encode.h"
#include "encoding/xor_encode.h"

std::string encode(const std::string& flag, const std::string& value);

#endif // ENCODING_FUNCTIONS_H
