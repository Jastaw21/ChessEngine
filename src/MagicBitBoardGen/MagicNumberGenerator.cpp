//
// Created by jacks on 10/07/2025.
//

#include "../../include/MagicBitBoardGen/MagicNumberGenerator.h"

#include <chrono>
#include <iostream>

uint64_t NUM_ATTEMPTS = 400000000;

MagicNumberGenerator::MagicNumberGenerator() : rng(std::chrono::steady_clock::now().time_since_epoch().count()){}


Bitboard MagicNumberGenerator::findMagicNumber(int square, bool is_rook){
    const Bitboard mask = is_rook ? mbbHelpers.generateRookMask(square) : mbbHelpers.generateBishopMask(square);

    const int maskBits = __builtin_popcountll(mask);
    // left shifting is increasing powers of 2, so the number of combinations there are of the number of possible bits set from this rank
    const int attacksSize = 1 << maskBits;

    std::vector<Bitboard> occupancies(attacksSize);
    std::vector<Bitboard> attacks(attacksSize);

    // now try each of these possible occupancies, and which attacks correspond
    for (int i = 0; i < attacksSize; i++) {
        occupancies[i] = mbbHelpers.getOccupancyFromIndex(i, mask);
        attacks[i] = is_rook ? mbbHelpers.getRookAttacks(square, occupancies[i]) : mbbHelpers.getBishopAttacks(square, occupancies[i]);
    }

    for (int attempt = 0; attempt < NUM_ATTEMPTS; attempt++) {
        const Bitboard magic = randomBitboard();

        if (__builtin_popcountll(mask * magic & 0xFF00000000000000ULL) < 6) {
            continue; // Skip if magic doesn't have enough bits in upper part
        }

        // Test if this magic number works
        std::vector<Bitboard> used_attacks(attacksSize, 0ULL);
        bool collision = false;

        for (int i = 0; i < attacksSize && !collision; i++) {
            const int index = (occupancies[i] * magic) >> (64 - maskBits);

            if (used_attacks[index] == 0ULL) { used_attacks[index] = attacks[i]; } else if (
                used_attacks[index] != attacks[i]) { collision = true; }
        }

        if (!collision) { return magic; }
    }

    std::cout << "Failed to find magic for " << (is_rook ? "rook" : "bishop")
            << " square " << square << std::endl;
    return 0ULL;
}
void MagicNumberGenerator::generateRookMagics(){
    std::cout << "Generating rook magic numbers..." << std::endl;
    std::cout << "const Bitboard rookMagicNumbers[64] = {" << std::endl;

    for (int square = 0; square < 64; square++) {
        const Bitboard magic = findMagicNumber(square, true);
        std::cout << "    0x" << std::hex << std::uppercase << magic << "ULL";
        if (square < 63) std::cout << ",";
        std::cout << " // " << std::dec << square << std::endl;
    }

    std::cout << "};" << std::endl;
}
void MagicNumberGenerator::generateBishopMagics(){
    std::cout << "Generating bishop magic numbers..." << std::endl;
    std::cout << "const Bitboard bishopMagicNumbers[64] = {" << std::endl;

    for (int square = 0; square < 64; square++) {
        const Bitboard magic = findMagicNumber(square, false);
        std::cout << "    0x" << std::hex << std::uppercase << magic << "ULL";
        if (square < 63) std::cout << ",";
        std::cout << " // " << std::dec << square << std::endl;
    }

    std::cout << "};" << std::endl;
}