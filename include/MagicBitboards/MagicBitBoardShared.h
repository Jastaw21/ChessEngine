//
// Created by jacks on 11/07/2025.
//

#ifndef MAGICBITBOARDSHARED_H
#define MAGICBITBOARDSHARED_H
#include "BoardManager/BitBoards.h"

class MBBHelpers {
public:

    Bitboard generateRookMask(int square);
    Bitboard generateBishopMask(int square);
    Bitboard getRookAttacks(int square, const Bitboard& occupancy);
    Bitboard getBishopAttacks(int square, const Bitboard& occupancy);


    Bitboard getOccupancyFromIndex(const int index, Bitboard mask);
};

#endif //MAGICBITBOARDSHARED_H
