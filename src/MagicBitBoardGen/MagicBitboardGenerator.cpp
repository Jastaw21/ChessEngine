//
// Created by jacks on 10/07/2025.
//

#include "../../include/MagicBitBoardGen/MagicBitboardGenerator.h"

#include <chrono>
#include <iostream>

uint64_t NUM_ATTEMPTS = 400000000;

MagicBitboardGenerator::MagicBitboardGenerator() : rng(std::chrono::steady_clock::now().time_since_epoch().count()){}

Bitboard MagicBitboardGenerator::generateRookMask(int square){
    Bitboard attacks = 0ULL;
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;

    for (int r = 2; r < 8; r++) {
        if (r != rank)
            attacks |= 1ULL << rankAndFileToSquare(r, file);
    }

    for (int f = 2; f < 8; f++) {
        if (f != file)
            attacks |= 1ULL << rankAndFileToSquare(rank, f);
    }

    return attacks;
}

Bitboard MagicBitboardGenerator::generateBishopMask(int square){
    Bitboard attacks = 0ULL;
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;

    struct MoveDir {
        int dRank;
        int dFile;
    };

    MoveDir moveDirs[] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    for (const auto& move: moveDirs) {
        int targetRank = rank + move.dRank;
        int targetFile = file + move.dFile;

        while (targetRank >= 2 && targetRank <= 7 && targetFile >= 2 && targetFile <= 7) {
            attacks |= 1ULL << rankAndFileToSquare(targetRank, targetFile);
            targetFile += move.dFile;
            targetRank += move.dRank;
        }
    }

    return attacks;
}

Bitboard MagicBitboardGenerator::getRookAttacks(int square, const Bitboard& occupancy){
    Bitboard attacks = 0ULL;
    const int rank = square / 8;
    const int file = square % 8;
    //Horizontal directions
    for (int f = file + 1; f < 8; f++) {
        attacks |= (1ULL << (rank * 8 + f));
        if (occupancy & (1ULL << (rank * 8 + f))) break;
    }
    for (int f = file - 1; f >= 0; f--) {
        attacks |= (1ULL << (rank * 8 + f));
        if (occupancy & (1ULL << (rank * 8 + f))) break;
    }

    // Vertical directions
    for (int r = rank + 1; r < 8; r++) {
        attacks |= (1ULL << (r * 8 + file));
        if (occupancy & (1ULL << (r * 8 + file))) break;
    }
    for (int r = rank - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + file));
        if (occupancy & (1ULL << (r * 8 + file))) break;
    }

    return attacks;
}

Bitboard MagicBitboardGenerator::getBishopAttacks(int square, const Bitboard& occupancy){
    Bitboard attacks = 0ULL;
    const int rank = square / 8;
    const int file = square % 8;

    const int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (int dir = 0; dir < 4; dir++) {
        int r = rank + directions[dir][0];
        int f = file + directions[dir][1];

        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            attacks |= (1ULL << (r * 8 + f));
            if (occupancy & (1ULL << (r * 8 + f))) break;
            r += directions[dir][0];
            f += directions[dir][1];
        }
    }

    return attacks;
}


Bitboard MagicBitboardGenerator::findMagicNumber(int square, bool is_rook){
    const Bitboard mask = is_rook ? generateRookMask(square) : generateBishopMask(square);

    const int maskBits = __builtin_popcountll(mask);
    // left shifting is increasing powers of 2, so the number of combinations there are of the number of possible bits set from this rank
    const int attacksSize = 1 << maskBits;

    std::vector<Bitboard> occupancies(attacksSize);
    std::vector<Bitboard> attacks(attacksSize);

    // now try each of these possible occupancies, and which attacks correspond
    for (int i = 0; i < attacksSize; i++) {
        occupancies[i] = getOccupancyFromIndex(i, mask);
        attacks[i] = is_rook ? getRookAttacks(square, occupancies[i]) : getBishopAttacks(square, occupancies[i]);
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


Bitboard MagicBitboardGenerator::getOccupancyFromIndex(const int index, Bitboard mask){
    Bitboard occupancy = 0ULL;
    const int numBits = __builtin_popcountll(mask);

    for (int i = 0; i < numBits; i++) {
        const int lowestBit = __builtin_ctzll(mask);
        mask &= mask - 1; // get rid of the lowest set bit in the mask

        if (index & 1 << i) {
            // if this index is set in the mask, add it to the bits
            occupancy |= 1ULL << lowestBit;
        }
    }

    return occupancy;
}

void MagicBitboardGenerator::generateRookMagics(){
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

void MagicBitboardGenerator::generateBishopMagics(){
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