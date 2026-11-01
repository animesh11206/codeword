#include <cstdint>
#include <string>
#include "html_entity_named_encode.h"
#include "encoding_utils.h"

// Replaces characters that have a named HTML entity (& < > " ' and Latin-1 U+00A0..U+00FF).
// Characters without a named entity are left unchanged.
std::string html_entity_named_encode(const std::string& value) {
    // Index 0 corresponds to U+00A0, index 95 to U+00FF
    static const char* const latin1_names[96] = {
        "nbsp",   "iexcl",  "cent",   "pound",  "curren", "yen",    "brvbar", "sect",
        "uml",    "copy",   "ordf",   "laquo",  "not",    "shy",    "reg",    "macr",
        "deg",    "plusmn", "sup2",   "sup3",   "acute",  "micro",  "para",   "middot",
        "cedil",  "sup1",   "ordm",   "raquo",  "frac14", "frac12", "frac34", "iquest",
        "Agrave", "Aacute", "Acirc",  "Atilde", "Auml",   "Aring",  "AElig",  "Ccedil",
        "Egrave", "Eacute", "Ecirc",  "Euml",   "Igrave", "Iacute", "Icirc",  "Iuml",
        "ETH",    "Ntilde", "Ograve", "Oacute", "Ocirc",  "Otilde", "Ouml",   "times",
        "Oslash", "Ugrave", "Uacute", "Ucirc",  "Uuml",   "Yacute", "THORN",  "szlig",
        "agrave", "aacute", "acirc",  "atilde", "auml",   "aring",  "aelig",  "ccedil",
        "egrave", "eacute", "ecirc",  "euml",   "igrave", "iacute", "icirc",  "iuml",
        "eth",    "ntilde", "ograve", "oacute", "ocirc",  "otilde", "ouml",   "divide",
        "oslash", "ugrave", "uacute", "ucirc",  "uuml",   "yacute", "thorn",  "yuml"
    };

    std::string encoded;

    for (std::uint32_t code_point : encoding_utils::decode_utf8(value)) {
        switch (code_point) {
            case '&':  encoded += "&amp;";  continue;
            case '<':  encoded += "&lt;";   continue;
            case '>':  encoded += "&gt;";   continue;
            case '"':  encoded += "&quot;"; continue;
            case '\'': encoded += "&apos;"; continue;
            default: break;
        }

        if (code_point >= 0xA0 && code_point <= 0xFF) {
            encoded += '&';
            encoded += latin1_names[code_point - 0xA0];
            encoded += ';';
        }
        else {
            encoding_utils::append_utf8(encoded, code_point);
        }
    }

    return encoded;
}

