#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include <filesystem>

const std::string AUTHOR;
const std::string VERSION;

const std::string ALL_FLAGS[] = {
    // Utility flags
    "-h", "--help",
    "-v", "--version",
    "-w", "--wordlist",
    "-o", "--output",

    // Operation flags
    "--encoding",
    "--decoding",
    "--hashing",

    // Encoding / Decoding sub-flags (36)
    "-ascii",
    "-base32",
    "-base32hex",
    "-base58",
    "-base62",
    "-base64",
    "-base64url",
    "-base85",
    "-binary",
    "-decimal",
    "-double_url",
    "-hex",
    "-hex_escape",
    "-html_entity_dec",
    "-html_entity_hex",
    "-html_entity_named",
    "-octal",
    "-octal_escape",
    "-punycode",
    "-quoted_printable",
    "-rot13",
    "-rot47",
    "-unicode_codepoint",
    "-unicode_escape",
    "-unicode_iis",
    "-url",
    "-url_all",
    "-utf8",
    "-utf16",
    "-utf16be",
    "-utf16le",
    "-utf32",
    "-utf32be",
    "-utf32le",
    "-uuencode",
    "-xor",

    // Hashing sub-flags (49)
    "-adler32",
    "-bcrypt",
    "-blake2b",
    "-blake2s",
    "-blake3",
    "-crc32",
    "-crc64",
    "-fnv1a_32",
    "-fnv1a_64",
    "-keccak256",
    "-keccak512",
    "-lm",
    "-md2",
    "-md4",
    "-md5",
    "-md5_crypt",
    "-mssql2000",
    "-mssql2005",
    "-mssql2012",
    "-mysql323",
    "-mysql41",
    "-ntlm",
    "-pbkdf2_sha1",
    "-pbkdf2_sha256",
    "-pbkdf2_sha512",
    "-postgres_md5",
    "-ripemd128",
    "-ripemd160",
    "-ripemd256",
    "-ripemd320",
    "-scrypt",
    "-sha1",
    "-sha224",
    "-sha256",
    "-sha256_crypt",
    "-sha384",
    "-sha512",
    "-sha512_crypt",
    "-sha3_224",
    "-sha3_256",
    "-sha3_384",
    "-sha3_512",
    "-shake128",
    "-shake256",
    "-sm3",
    "-tiger192",
    "-whirlpool",
    "-xxhash32",
    "-xxhash64"
};

const std::string UTILITY_FLAGS[] = {
    "-h", "--help",
    "-v", "--version",
    "-w", "--wordlist",
    "-o", "--output"
};

const std::string ENCODING_FLAGS[] = {
    "-ascii",
    "-base32",
    "-base32hex",
    "-base58",
    "-base62",
    "-base64",
    "-base64url",
    "-base85",
    "-binary",
    "-decimal",
    "-double_url",
    "-hex",
    "-hex_escape",
    "-html_entity_dec",
    "-html_entity_hex",
    "-html_entity_named",
    "-octal",
    "-octal_escape",
    "-punycode",
    "-quoted_printable",
    "-rot13",
    "-rot47",
    "-unicode_codepoint",
    "-unicode_escape",
    "-unicode_iis",
    "-url",
    "-url_all",
    "-utf8",
    "-utf16",
    "-utf16be",
    "-utf16le",
    "-utf32",
    "-utf32be",
    "-utf32le",
    "-uuencode",
    "-xor"
};

const std::string DECODING_FLAGS[] = {
    "-ascii",
    "-base32",
    "-base32hex",
    "-base58",
    "-base62",
    "-base64",
    "-base64url",
    "-base85",
    "-binary",
    "-decimal",
    "-double_url",
    "-hex",
    "-hex_escape",
    "-html_entity_dec",
    "-html_entity_hex",
    "-html_entity_named",
    "-octal",
    "-octal_escape",
    "-punycode",
    "-quoted_printable",
    "-rot13",
    "-rot47",
    "-unicode_codepoint",
    "-unicode_escape",
    "-unicode_iis",
    "-url",
    "-url_all",
    "-utf8",
    "-utf16",
    "-utf16be",
    "-utf16le",
    "-utf32",
    "-utf32be",
    "-utf32le",
    "-uuencode",
    "-xor"
};

const std::string HASHING_FLAGS[] = {
    "-adler32",
    "-bcrypt",
    "-blake2b",
    "-blake2s",
    "-blake3",
    "-crc32",
    "-crc64",
    "-fnv1a_32",
    "-fnv1a_64",
    "-keccak256",
    "-keccak512",
    "-lm",
    "-md2",
    "-md4",
    "-md5",
    "-md5_crypt",
    "-mssql2000",
    "-mssql2005",
    "-mssql2012",
    "-mysql323",
    "-mysql41",
    "-ntlm",
    "-pbkdf2_sha1",
    "-pbkdf2_sha256",
    "-pbkdf2_sha512",
    "-postgres_md5",
    "-ripemd128",
    "-ripemd160",
    "-ripemd256",
    "-ripemd320",
    "-scrypt",
    "-sha1",
    "-sha224",
    "-sha256",
    "-sha256_crypt",
    "-sha384",
    "-sha512",
    "-sha512_crypt",
    "-sha3_224",
    "-sha3_256",
    "-sha3_384",
    "-sha3_512",
    "-shake128",
    "-shake256",
    "-sm3",
    "-tiger192",
    "-whirlpool",
    "-xxhash32",
    "-xxhash64"
};

const int NUM_ALL_FLAGS = sizeof(ALL_FLAGS) / sizeof(ALL_FLAGS[0]);
const int NUM_UTILITY_FLAGS = sizeof(UTILITY_FLAGS) / sizeof(UTILITY_FLAGS[0]);
const int NUM_ENCODING_FLAGS = sizeof(ENCODING_FLAGS) / sizeof(ENCODING_FLAGS[0]);
const int NUM_DECODING_FLAGS = sizeof(DECODING_FLAGS) / sizeof(DECODING_FLAGS[0]);
const int NUM_HASHING_FLAGS = sizeof(HASHING_FLAGS) / sizeof(HASHING_FLAGS[0]);

std::string input_file;
std::string output_file;

bool append_output = false;
bool output_file_creation = false;

std::string operations[3] = {"--encoding", "--decoding", "--hashing"};
std::string used_operation_flags[5] = {};

int max_used_encoding_flags = 5;
int max_used_decoding_flags = 1;
int max_used_hashing_flags = 5;
int num_operation_flags_used = 0;

