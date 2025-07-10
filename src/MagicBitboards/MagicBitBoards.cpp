//
// Created by jacks on 10/07/2025.
//

#include "MagicBitboards/MagicBitBoards.h"

void MagicBitBoards::initRookMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = rookMagics[square];
        magic.mask = rookMagicNumbers[square]; // Use precomputed magic
        magic.shift = 64 - __builtin_popcountll(magic.mask);
    }
}

void MagicBitBoards::initBishopMagics(){}

Bitboard MagicBitBoards::generateBishopMask(int square){
    Bitboard mask = 0ULL;
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;
    // Diagonal directions
    const int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (int dir = 0; dir < 4; dir++) {
        int r = rank + directions[dir][0];
        int f = file + directions[dir][1];

        while (r >= 1 && r <= 6 && f >= 1 && f <= 6) {
            mask |= (1ULL << (r * 8 + f));
            r += directions[dir][0];
            f += directions[dir][1];
        }
    }

    return mask;
}

Bitboard MagicBitBoards::generateRookMask(const int square){
    Bitboard mask = 0ULL;
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;
    //Horizontal (excluding edge files)
    for (int f = 2; f < 8; f++) { if (f != file) { mask |= 1ULL << (rank * 8 + f); } }

    // Vertical (excluding edge ranks)
    for (int r = 2; r < 8; r++) { if (r != rank) { mask |= 1ULL << (r * 8 + file); } }

    return mask;
}