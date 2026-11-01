#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include <filesystem>

#include "command_validation.h"
#include "display_help.h"
#include "input_file_validation.h"
#include "output_file_validation.h"
#include "file_collision_handling.h"
#include "output_file_creation.h"
#include "operation_validation.h"
#include "encoding_functions.h"
#include "decoding_functions.h"
#include "hashing_functions.h"

const std::string AUTHOR = "Animesh Bhatia";
const std::string VERSION = "1.0.0";

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

int main(int argc, char* argv[]) {

    if (argc == 2) {
        std::string arg = argv[1];

        if (arg == "-h" || arg == "--help") {
            display_help(argv[0]);
            return 0;
        }

        if (arg == "-v" || arg == "--version") {
            display_version();
            return 0;
        }
    }

    if (!Command_Validation(argc, argv)) {
        return 1;
    }

    std::string active_operation;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--encoding" || arg == "--decoding" || arg == "--hashing") {
            active_operation = arg;
            break;
        }
    }

    if (output_file.empty() || output_file_creation) {
        if (!Output_File_Creation(input_file, used_operation_flags)) {
            return 1;
        }
    }

    std::error_code eq_ec;
    if (std::filesystem::equivalent(input_file, output_file, eq_ec)) {
        std::cerr << "Error: Output file '" << output_file << "' cannot be the same as input file '" << input_file << "'." << std::endl;
        return 1;
    }

    std::ifstream in_stream(input_file, std::ios::in | std::ios::binary);
    if (!in_stream.is_open()) {
        std::cerr << "Error: Unable to open input file '" << input_file << "'." << std::endl;
        return 1;
    }

    std::ofstream out_stream;
    if (append_output) {
        out_stream.open(output_file, std::ios::out | std::ios::binary | std::ios::app);
    } else {
        out_stream.open(output_file, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    if (!out_stream.is_open()) {
        std::cerr << "Error: Unable to open output file '" << output_file << "'." << std::endl;
        return 1;
    }

    try {
        std::string line;
        while (std::getline(in_stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            std::string current = line;
            for (int i = 0; i < num_operation_flags_used; ++i) {
                if (active_operation == "--encoding") {
                    current = encode(used_operation_flags[i], current);
                } else if (active_operation == "--decoding") {
                    current = decode(used_operation_flags[i], current);
                } else if (active_operation == "--hashing") {
                    current = hash(used_operation_flags[i], current);
                }
            }

            out_stream << current << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
