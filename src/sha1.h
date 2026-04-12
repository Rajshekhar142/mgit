#pragma once
#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>

// SHA1 — implemented from scratch, no dependencies.
//
// SHA1 takes any input and produces a fixed 40-char hex string.
// Same input ALWAYS gives same output. Change one byte → completely
// different hash. That's what makes it perfect as an object ID.
//
// How it works (3 steps):
//   1. PAD    — make message length a multiple of 512 bits
//   2. CHUNK  — process message in 64-byte blocks
//   3. MIX    — 80 rounds of bit operations per block → 160-bit result

class SHA1 {
public:
    static std::string hash(const std::string& input) {

        // --- Initial hash values (constants defined by SHA1 spec) ---
        uint32_t h0 = 0x67452301;
        uint32_t h1 = 0xEFCDAB89;
        uint32_t h2 = 0x98BADCFE;
        uint32_t h3 = 0x10325476;
        uint32_t h4 = 0xC3D2E1F0;

        // --- STEP 1: PAD the message ---
        // Append 0x80, then zeros, then original bit-length as 8 bytes
        // Goal: total length becomes a multiple of 64 bytes (512 bits)
        std::string msg = input;
        uint64_t originalBitLen = input.size() * 8;

        msg += (char)0x80; // mark end of real content
        while (msg.size() % 64 != 56)
            msg += (char)0x00; // pad with zeros

        // append original length as big-endian 64-bit integer
        for (int i = 7; i >= 0; i--)
            msg += (char)((originalBitLen >> (i * 8)) & 0xFF);

        // --- STEP 2 + 3: Process each 64-byte chunk ---
        for (size_t i = 0; i < msg.size(); i += 64) {

            uint32_t w[80]; // message schedule — 80 words derived from chunk

            // First 16 words: read directly from chunk (big-endian)
            for (int j = 0; j < 16; j++) {
                w[j] = ((uint8_t)msg[i + j*4]     << 24) |
                       ((uint8_t)msg[i + j*4 + 1] << 16) |
                       ((uint8_t)msg[i + j*4 + 2] <<  8) |
                       ((uint8_t)msg[i + j*4 + 3]);
            }

            // Remaining 64 words: derived from previous words via XOR + rotate
            // This is what "mixes" the content deeply
            for (int j = 16; j < 80; j++) {
                uint32_t val = w[j-3] ^ w[j-8] ^ w[j-14] ^ w[j-16];
                w[j] = (val << 1) | (val >> 31); // left rotate by 1
            }

            // Working variables — start from current hash state
            uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;

            // 80 rounds of mixing
            // Each round uses a different function f and constant k
            for (int j = 0; j < 80; j++) {
                uint32_t f, k;

                if (j < 20) {
                    f = (b & c) | (~b & d); // Choice: if b then c else d
                    k = 0x5A827999;
                } else if (j < 40) {
                    f = b ^ c ^ d;          // Parity
                    k = 0x6ED9EBA1;
                } else if (j < 60) {
                    f = (b & c) | (b & d) | (c & d); // Majority vote
                    k = 0x8F1BBCDC;
                } else {
                    f = b ^ c ^ d;          // Parity again
                    k = 0xCA62C1D6;
                }

                uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[j];
                e = d;
                d = c;
                c = (b << 30) | (b >> 2); // right rotate b by 2
                b = a;
                a = temp;
            }

            // Add this chunk's result into the running hash
            h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
        }

        // --- Produce final 40-char hex string from 5 uint32 values ---
        std::ostringstream oss;
        oss << std::hex << std::setfill('0')
            << std::setw(8) << h0
            << std::setw(8) << h1
            << std::setw(8) << h2
            << std::setw(8) << h3
            << std::setw(8) << h4;
        return oss.str();
    }
};