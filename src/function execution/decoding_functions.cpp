#include <string>
#include <stdexcept>
#include "decoding_functions.h"

std::string decode(const std::string& flag, const std::string& value) {

    if (flag == "-ascii") {
        return ascii_decode(value);
    }

    if (flag == "-base32") {
        return base32_decode(value);
    }

    if (flag == "-base32hex") {
        return base32hex_decode(value);
    }

    if (flag == "-base58") {
        return base58_decode(value);
    }

    if (flag == "-base62") {
        return base62_decode(value);
    }

    if (flag == "-base64") {
        return base64_decode(value);
    }

    if (flag == "-base64url") {
        return base64url_decode(value);
    }

    if (flag == "-base85") {
        return base85_decode(value);
    }

    if (flag == "-binary") {
        return binary_decode(value);
    }

    if (flag == "-decimal") {
        return decimal_decode(value);
    }

    if (flag == "-double_url") {
        return double_url_decode(value);
    }

    if (flag == "-hex") {
        return hex_decode(value);
    }

    if (flag == "-hex_escape") {
        return hex_escape_decode(value);
    }

    if (flag == "-html_entity_dec") {
        return html_entity_dec_decode(value);
    }

    if (flag == "-html_entity_hex") {
        return html_entity_hex_decode(value);
    }

    if (flag == "-html_entity_named") {
        return html_entity_named_decode(value);
    }

    if (flag == "-octal") {
        return octal_decode(value);
    }

    if (flag == "-octal_escape") {
        return octal_escape_decode(value);
    }

    if (flag == "-punycode") {
        return punycode_decode(value);
    }

    if (flag == "-quoted_printable") {
        return quoted_printable_decode(value);
    }

    if (flag == "-rot13") {
        return rot13_decode(value);
    }

    if (flag == "-rot47") {
        return rot47_decode(value);
    }

    if (flag == "-unicode_codepoint") {
        return unicode_codepoint_decode(value);
    }

    if (flag == "-unicode_escape") {
        return unicode_escape_decode(value);
    }

    if (flag == "-unicode_iis") {
        return unicode_iis_decode(value);
    }

    if (flag == "-url") {
        return url_decode(value);
    }

    if (flag == "-url_all") {
        return url_all_decode(value);
    }

    if (flag == "-utf8") {
        return utf8_decode(value);
    }

    if (flag == "-utf16") {
        return utf16_decode(value);
    }

    if (flag == "-utf16be") {
        return utf16be_decode(value);
    }

    if (flag == "-utf16le") {
        return utf16le_decode(value);
    }

    if (flag == "-utf32") {
        return utf32_decode(value);
    }

    if (flag == "-utf32be") {
        return utf32be_decode(value);
    }

    if (flag == "-utf32le") {
        return utf32le_decode(value);
    }

    if (flag == "-uuencode") {
        return uuencode_decode(value);
    }

    if (flag == "-xor") {
        return xor_decode(value);
    }

    throw std::invalid_argument("Unknown decoding flag: " + flag);
}

