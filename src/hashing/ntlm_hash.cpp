#include <string>
#include "ntlm_hash.h"
#include "hashing_utils.h"

// Windows NTLM hash: MD4(UTF-16LE(value)) formatted as 32 lowercase hex characters.
std::string ntlm_hash(const std::string& value) {
    std::string utf16le = hashing_utils::to_utf16le_bytes(value);
    auto digest = hashing_utils::md4_raw(reinterpret_cast<const unsigned char*>(utf16le.data()), utf16le.size());
    return hashing_utils::bytes_to_hex(digest);
}

