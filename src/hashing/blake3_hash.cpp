#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include "blake3_hash.h"
#include "hashing_utils.h"

namespace {

constexpr std::uint32_t BLAKE3_IV[8] = {
    0x6A09E667U, 0xBB67AE85U, 0x3C6EF372U, 0xA54FF53AU,
    0x510E527FU, 0x9B05688CU, 0x1F83D9ABU, 0x5BE0CD19U
};

constexpr std::uint32_t CHUNK_START = 1U << 0;
constexpr std::uint32_t CHUNK_END   = 1U << 1;
constexpr std::uint32_t PARENT      = 1U << 2;
constexpr std::uint32_t ROOT        = 1U << 3;

constexpr unsigned char MSG_PERMUTATION[16] = {
    2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8
};

void g(std::uint32_t state[16], int a, int b, int c, int d, std::uint32_t mx, std::uint32_t my) {
    state[a] = state[a] + state[b] + mx;
    state[d] = hashing_utils::rotr32(state[d] ^ state[a], 16);
    state[c] = state[c] + state[d];
    state[b] = hashing_utils::rotr32(state[b] ^ state[c], 12);
    state[a] = state[a] + state[b] + my;
    state[d] = hashing_utils::rotr32(state[d] ^ state[a], 8);
    state[c] = state[c] + state[d];
    state[b] = hashing_utils::rotr32(state[b] ^ state[c], 7);
}

void round_fn(std::uint32_t state[16], const std::uint32_t m[16]) {
    g(state, 0, 4,  8, 12, m[0],  m[1]);
    g(state, 1, 5,  9, 13, m[2],  m[3]);
    g(state, 2, 6, 10, 14, m[4],  m[5]);
    g(state, 3, 7, 11, 15, m[6],  m[7]);
    g(state, 0, 5, 10, 15, m[8],  m[9]);
    g(state, 1, 6, 11, 12, m[10], m[11]);
    g(state, 2, 7,  8, 13, m[12], m[13]);
    g(state, 3, 4,  9, 14, m[14], m[15]);
}

void permute(std::uint32_t m[16]) {
    std::uint32_t permuted[16];
    for (int i = 0; i < 16; ++i) {
        permuted[i] = m[MSG_PERMUTATION[i]];
    }
    std::memcpy(m, permuted, sizeof(permuted));
}

std::array<std::uint32_t, 8> compress(const std::array<std::uint32_t, 8>& cv,
                                      const unsigned char block[64],
                                      std::uint32_t block_len,
                                      std::uint64_t counter,
                                      std::uint32_t flags) {
    std::uint32_t m[16];
    for (int i = 0; i < 16; ++i) {
        m[i] = hashing_utils::load_le32(block + i * 4);
    }

    std::uint32_t state[16] = {
        cv[0], cv[1], cv[2], cv[3], cv[4], cv[5], cv[6], cv[7],
        BLAKE3_IV[0], BLAKE3_IV[1], BLAKE3_IV[2], BLAKE3_IV[3],
        static_cast<std::uint32_t>(counter & 0xFFFFFFFFULL),
        static_cast<std::uint32_t>(counter >> 32),
        block_len,
        flags
    };

    for (int r = 0; r < 7; ++r) {
        round_fn(state, m);
        if (r < 6) {
            permute(m);
        }
    }

    std::array<std::uint32_t, 8> out;
    for (int i = 0; i < 8; ++i) {
        out[i] = state[i] ^ state[i + 8];
    }
    return out;
}

struct OutputNode {
    std::array<std::uint32_t, 8> input_cv;
    unsigned char block[64];
    std::uint32_t block_len;
    std::uint64_t counter;
    std::uint32_t flags;

    std::array<std::uint32_t, 8> chaining_value() const {
        return compress(input_cv, block, block_len, counter, flags);
    }

    std::array<std::uint32_t, 8> root_words() const {
        return compress(input_cv, block, block_len, 0, flags | ROOT);
    }
};

OutputNode chunk_output(const unsigned char* data, std::size_t len, std::uint64_t chunk_counter) {
    std::array<std::uint32_t, 8> cv;
    for (int i = 0; i < 8; ++i) cv[i] = BLAKE3_IV[i];

    std::size_t offset = 0;
    std::size_t blocks_compressed = 0;

    while (len - offset > 64) {
        std::uint32_t flags = (blocks_compressed == 0) ? CHUNK_START : 0;
        cv = compress(cv, data + offset, 64, chunk_counter, flags);
        offset += 64;
        ++blocks_compressed;
    }

    OutputNode node{};
    node.input_cv = cv;
    std::memset(node.block, 0, 64);
    std::size_t rem = len - offset;
    if (rem > 0) {
        std::memcpy(node.block, data + offset, rem);
    }
    node.block_len = static_cast<std::uint32_t>(rem);
    node.counter = chunk_counter;
    node.flags = CHUNK_END | (blocks_compressed == 0 ? CHUNK_START : 0);
    return node;
}

OutputNode parent_output(const std::array<std::uint32_t, 8>& left,
                         const std::array<std::uint32_t, 8>& right) {
    OutputNode node{};
    for (int i = 0; i < 8; ++i) node.input_cv[i] = BLAKE3_IV[i];
    for (int i = 0; i < 8; ++i) {
        hashing_utils::store_le32(node.block + i * 4, left[i]);
        hashing_utils::store_le32(node.block + 32 + i * 4, right[i]);
    }
    node.block_len = 64;
    node.counter = 0;
    node.flags = PARENT;
    return node;
}

} // namespace

// Official BLAKE3 256-bit cryptographic hash (64 lowercase hex characters).
std::string blake3_hash(const std::string& value) {
    const unsigned char* data = reinterpret_cast<const unsigned char*>(value.data());
    std::size_t len = value.size();

    if (len <= 1024) {
        OutputNode root = chunk_output(data, len, 0);
        auto words = root.root_words();
        unsigned char out[32];
        for (int i = 0; i < 8; ++i) hashing_utils::store_le32(out + i * 4, words[i]);
        return hashing_utils::bytes_to_hex(out, 32);
    }

    std::vector<std::array<std::uint32_t, 8>> cv_stack;
    std::uint64_t chunk_counter = 0;
    std::size_t offset = 0;

    while (offset + 1024 < len) {
        auto new_cv = chunk_output(data + offset, 1024, chunk_counter).chaining_value();
        std::uint64_t total_chunks = ++chunk_counter;
        while ((total_chunks & 1) == 0) {
            new_cv = parent_output(cv_stack.back(), new_cv).chaining_value();
            cv_stack.pop_back();
            total_chunks >>= 1;
        }
        cv_stack.push_back(new_cv);
        offset += 1024;
    }

    OutputNode out_node = chunk_output(data + offset, len - offset, chunk_counter);
    while (!cv_stack.empty()) {
        out_node = parent_output(cv_stack.back(), out_node.chaining_value());
        cv_stack.pop_back();
    }

    auto words = out_node.root_words();
    unsigned char out[32];
    for (int i = 0; i < 8; ++i) hashing_utils::store_le32(out + i * 4, words[i]);
    return hashing_utils::bytes_to_hex(out, 32);
}

