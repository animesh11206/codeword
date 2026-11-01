#include <cstdint>
#include <string>
#include "html_entity_named_decode.h"
#include "decoding_utils.h"

// Decodes named HTML entities (&amp;, &lt;, &gt;, &quot;, &apos;, and Latin-1 U+00A0..U+00FF).
std::string html_entity_named_decode(const std::string& value) {
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

    std::string decoded;

    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '&') {
            std::size_t semi = value.find(';', i + 1);
            if (semi != std::string::npos && semi - i <= 12) {
                std::string name = value.substr(i + 1, semi - i - 1);

                if (name == "amp")       { decoded += '&';  i = semi; continue; }
                else if (name == "lt")   { decoded += '<';  i = semi; continue; }
                else if (name == "gt")   { decoded += '>';  i = semi; continue; }
                else if (name == "quot") { decoded += '"';  i = semi; continue; }
                else if (name == "apos") { decoded += '\''; i = semi; continue; }

                bool matched = false;
                for (std::uint32_t idx = 0; idx < 96; ++idx) {
                    if (name == latin1_names[idx]) {
                        decoding_utils::append_utf8(decoded, 0xA0 + idx);
                        i = semi;
                        matched = true;
                        break;
                    }
                }
                if (matched) {
                    continue;
                }
            }
        }

        decoded += value[i];
    }

    return decoded;
}

