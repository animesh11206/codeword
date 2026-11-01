#include <string>
#include <stdexcept>
#include "encoding_functions.h"

std::string encode(const std::string& flag, const std::string& value) {

    if (flag == "-ascii") {
        return ascii_encode(value);
    }

    if (flag == "-base32") {
        return base32_encode(value);
    }

    if (flag == "-base32hex") {
        return base32hex_encode(value);
    }

    if (flag == "-base58") {
        return base58_encode(value);
    }

    if (flag == "-base62") {
        return base62_encode(value);
    }

    if (flag == "-base64") {
        return base64_encode(value);
    }

    if (flag == "-base64url") {
        return base64url_encode(value);
    }

    if (flag == "-base85") {
        return base85_encode(value);
    }

    if (flag == "-binary") {
        return binary_encode(value);
    }

    if (flag == "-decimal") {
        return decimal_encode(value);
    }

    if (flag == "-double_url") {
        return double_url_encode(value);
    }

    if (flag == "-hex") {
        return hex_encode(value);
    }

    if (flag == "-hex_escape") {
        return hex_escape_encode(value);
    }

    if (flag == "-html_entity_dec") {
        return html_entity_dec_encode(value);
    }

    if (flag == "-html_entity_hex") {
        return html_entity_hex_encode(value);
    }

    if (flag == "-html_entity_named") {
        return html_entity_named_encode(value);
    }

    if (flag == "-octal") {
        return octal_encode(value);
    }

    if (flag == "-octal_escape") {
        return octal_escape_encode(value);
    }

    if (flag == "-punycode") {
        return punycode_encode(value);
    }

    if (flag == "-quoted_printable") {
        return quoted_printable_encode(value);
    }

    if (flag == "-rot13") {
        return rot13_encode(value);
    }

    if (flag == "-rot47") {
        return rot47_encode(value);
    }

    if (flag == "-unicode_codepoint") {
        return unicode_codepoint_encode(value);
    }

    if (flag == "-unicode_escape") {
        return unicode_escape_encode(value);
    }

    if (flag == "-unicode_iis") {
        return unicode_iis_encode(value);
    }

    if (flag == "-url") {
        return url_encode(value);
    }

    if (flag == "-url_all") {
        return url_all_encode(value);
    }

    if (flag == "-utf8") {
        return utf8_encode(value);
    }

    if (flag == "-utf16") {
        return utf16_encode(value);
    }

    if (flag == "-utf16be") {
        return utf16be_encode(value);
    }

    if (flag == "-utf16le") {
        return utf16le_encode(value);
    }

    if (flag == "-utf32") {
        return utf32_encode(value);
    }

    if (flag == "-utf32be") {
        return utf32be_encode(value);
    }

    if (flag == "-utf32le") {
        return utf32le_encode(value);
    }

    if (flag == "-uuencode") {
        return uuencode_encode(value);
    }

    if (flag == "-xor") {
        return xor_encode(value);
    }

    throw std::invalid_argument("Unknown encoding flag: " + flag);
}

