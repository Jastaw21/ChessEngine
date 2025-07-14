//
// Created by jacks on 11/07/2025.
//

#include "MagicBitboards/MagicBitBoardShared.h"

#include <iostream>

Bitboard MBBHelpers::generateRookMask(int square){
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

Bitboard MBBHelpers::generateBishopMask(int square){
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

Bitboard MBBHelpers::getRookAttacks(int square, const Bitboard& occupancy){
    Bitboard attacks = 0ULL;
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;
    //Horizontal directions
    for (int f = file + 1; f <= 8; f++) {
        attacks |= 1ULL << rankAndFileToSquare(rank, f);
        if (occupancy & 1ULL << rankAndFileToSquare(rank, f)) break;
    }
    for (int f = file - 1; f > 0; f--) {
        attacks |= 1ULL << rankAndFileToSquare(rank, f);
        if (occupancy & 1ULL << rankAndFileToSquare(rank, f)) break;
    }

    // Vertical directions
    for (int r = rank + 1; r <= 8; r++) {
        attacks |= 1ULL << rankAndFileToSquare(r, file);
        if (occupancy & 1ULL << rankAndFileToSquare(r, file)) break;
    }
    for (int r = rank - 1; r > 0; r--) {
        attacks |= 1ULL << rankAndFileToSquare(r, file);;
        if (occupancy & 1ULL << rankAndFileToSquare(r, file)) break;
    }

    return attacks;
}

Bitboard MBBHelpers::getBishopAttacks(int square, const Bitboard& occupancy){
    Bitboard attacks = 0ULL;
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;

    const int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (int dir = 0; dir < 4; dir++) {
        int r = rank + directions[dir][0];
        int f = file + directions[dir][1];

        while (r > 0 && r <= 8 && f > 0 && f <= 8) {
            attacks |= 1ULL << rankAndFileToSquare(r, f);
            if (occupancy & 1ULL << rankAndFileToSquare(r, f)) break;
            r += directions[dir][0];
            f += directions[dir][1];
        }
    }

    return attacks;
}

Bitboard MBBHelpers::getOccupancyFromIndex(const int index, Bitboard mask){
    Bitboard occupancy = 0ULL;
    const int numBits = std::popcount(mask);

    for (int i = 0; i < numBits; i++) {
        const int lowestBit = std::countr_zero(mask);
        mask &= mask - 1; // get rid of the lowest set bit in the mask

        if (index & 1 << i) {
            // if this index is set in the mask, add it to the bits
            occupancy |= 1ULL << lowestBit;
        }
    }

    return occupancy;
}