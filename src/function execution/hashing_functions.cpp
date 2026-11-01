#include <string>
#include <stdexcept>
#include "hashing_functions.h"

std::string hash(const std::string& flag, const std::string& value) {

    if (flag == "-adler32") {
        return adler32_hash(value);
    }

    if (flag == "-bcrypt") {
        return bcrypt_hash(value);
    }

    if (flag == "-blake2b") {
        return blake2b_hash(value);
    }

    if (flag == "-blake2s") {
        return blake2s_hash(value);
    }

    if (flag == "-blake3") {
        return blake3_hash(value);
    }

    if (flag == "-crc32") {
        return crc32_hash(value);
    }

    if (flag == "-crc64") {
        return crc64_hash(value);
    }

    if (flag == "-fnv1a_32") {
        return fnv1a_32_hash(value);
    }

    if (flag == "-fnv1a_64") {
        return fnv1a_64_hash(value);
    }

    if (flag == "-keccak256") {
        return keccak256_hash(value);
    }

    if (flag == "-keccak512") {
        return keccak512_hash(value);
    }

    if (flag == "-lm") {
        return lm_hash(value);
    }

    if (flag == "-md2") {
        return md2_hash(value);
    }

    if (flag == "-md4") {
        return md4_hash(value);
    }

    if (flag == "-md5") {
        return md5_hash(value);
    }

    if (flag == "-md5_crypt") {
        return md5_crypt_hash(value);
    }

    if (flag == "-mssql2000") {
        return mssql2000_hash(value);
    }

    if (flag == "-mssql2005") {
        return mssql2005_hash(value);
    }

    if (flag == "-mssql2012") {
        return mssql2012_hash(value);
    }

    if (flag == "-mysql323") {
        return mysql323_hash(value);
    }

    if (flag == "-mysql41") {
        return mysql41_hash(value);
    }

    if (flag == "-ntlm") {
        return ntlm_hash(value);
    }

    if (flag == "-pbkdf2_sha1") {
        return pbkdf2_sha1_hash(value);
    }

    if (flag == "-pbkdf2_sha256") {
        return pbkdf2_sha256_hash(value);
    }

    if (flag == "-pbkdf2_sha512") {
        return pbkdf2_sha512_hash(value);
    }

    if (flag == "-postgres_md5") {
        return postgres_md5_hash(value);
    }

    if (flag == "-ripemd128") {
        return ripemd128_hash(value);
    }

    if (flag == "-ripemd160") {
        return ripemd160_hash(value);
    }

    if (flag == "-ripemd256") {
        return ripemd256_hash(value);
    }

    if (flag == "-ripemd320") {
        return ripemd320_hash(value);
    }

    if (flag == "-scrypt") {
        return scrypt_hash(value);
    }

    if (flag == "-sha1") {
        return sha1_hash(value);
    }

    if (flag == "-sha224") {
        return sha224_hash(value);
    }

    if (flag == "-sha256") {
        return sha256_hash(value);
    }

    if (flag == "-sha256_crypt") {
        return sha256_crypt_hash(value);
    }

    if (flag == "-sha384") {
        return sha384_hash(value);
    }

    if (flag == "-sha512") {
        return sha512_hash(value);
    }

    if (flag == "-sha512_crypt") {
        return sha512_crypt_hash(value);
    }

    if (flag == "-sha3_224") {
        return sha3_224_hash(value);
    }

    if (flag == "-sha3_256") {
        return sha3_256_hash(value);
    }

    if (flag == "-sha3_384") {
        return sha3_384_hash(value);
    }

    if (flag == "-sha3_512") {
        return sha3_512_hash(value);
    }

    if (flag == "-shake128") {
        return shake128_hash(value);
    }

    if (flag == "-shake256") {
        return shake256_hash(value);
    }

    if (flag == "-sm3") {
        return sm3_hash(value);
    }

    if (flag == "-tiger192") {
        return tiger192_hash(value);
    }

    if (flag == "-whirlpool") {
        return whirlpool_hash(value);
    }

    if (flag == "-xxhash32") {
        return xxhash32_hash(value);
    }

    if (flag == "-xxhash64") {
        return xxhash64_hash(value);
    }

    throw std::invalid_argument("Unknown hashing flag: " + flag);
}

