//
// Created by jacks on 10/07/2025.
//

#include "MagicBitboards/MagicBitBoards.h"

void MagicBitBoards::initRookMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = rookMagics[square];
        magic.mask = mbbHelpers.generateRookMask(square); // Use precomputed magic
        magic.magic = rookMagicNumbers[square];
        magic.shift = 64 - __builtin_popcountll(magic.mask); // keep relevant bits

        const int attacksSize = 1 << __builtin_popcountll(magic.mask); // how many attacks are there? shifting is powers
        magic.attacks.resize(attacksSize); // the max size of the table is the number of attacks

        for (int i = 0; i < attacksSize; i++) {
            Bitboard occupancy = mbbHelpers.getOccupancyFromIndex(i, magic.mask);
            const Bitboard attacks = mbbHelpers.getRookAttacks(square, occupancy);

            occupancy *= magic.magic; // apply the magic number
            occupancy >>= magic.shift; // shift it down
            magic.attacks[occupancy] = attacks;
        }
    }
}

void MagicBitBoards::initBishopMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = bishopMagics[square];
        magic.mask = mbbHelpers.generateBishopMask(square);
        magic.magic = bishopMagicNumbers[square]; // Use precomputed magic
        magic.shift = 64 - __builtin_popcountll(magic.mask); // keep relevant bits

        const int attacksSize = 1 << __builtin_popcountll(magic.mask); // how many attacks are there? shifting is powers
        magic.attacks.resize(attacksSize); // the max size of the table is the number of attacks

        for (int i = 0; i < attacksSize; i++) {
            Bitboard occupancy = mbbHelpers.getOccupancyFromIndex(i, magic.mask);
            const Bitboard attacks = mbbHelpers.getBishopAttacks(square, occupancy);

            occupancy *= magic.magic; // apply the magic number
            occupancy >>= magic.shift; // shift it down
            magic.attacks[occupancy] = attacks; // [ occupancy ] = [ attacks
        }
    }
}

Bitboard MagicBitBoards::getRookAttacks(const int square, Bitboard occupancy){
    Magic& magic = rookMagics[square];
    occupancy &= magic.mask;
    occupancy *= magic.magic;
    occupancy >>= magic.shift;
    return magic.attacks[occupancy];
}

Bitboard MagicBitBoards::getBishopAttacks(const int square, Bitboard occupancy){
    Magic& magic = bishopMagics[square];
    occupancy &= magic.mask;
    occupancy *= magic.magic;
    occupancy >>= magic.shift;
    return magic.attacks[occupancy];
}
