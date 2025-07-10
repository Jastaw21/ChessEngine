//
// Created by jacks on 10/07/2025.
//

#ifndef MAGICBITBOARDGENERATOR_H
#define MAGICBITBOARDGENERATOR_H
#include <random>

#include "BoardManager/BitBoards.h"


class MagicBitboardGenerator {
    std::mt19937_64 rng;

public:

    MagicBitboardGenerator();

    Bitboard randomBitboard(){ return rng() & rng() & rng(); }

    Bitboard generateRookMask(int square);
    Bitboard generateBishopMask(int square);
    Bitboard getRookAttacks(int square, const Bitboard& occupancy);
    Bitboard getBishopAttacks(int square, const Bitboard& occupancy);
    Bitboard findMagicNumber(int square, bool is_rook);

    Bitboard getOccupancyFromIndex(const int index, Bitboard mask);


    void generateMagicNumbers(){
        generateRookMagics();
        generateBishopMagics();
    };
    void generateRookMagics();
    void generateBishopMagics();

private:
};


#endif //MAGICBITBOARDGENERATOR_H
