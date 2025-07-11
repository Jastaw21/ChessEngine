//
// Created by jacks on 10/07/2025.
//

#ifndef MAGICBITBOARDGENERATOR_H
#define MAGICBITBOARDGENERATOR_H
#include <random>
#include "BoardManager/BitBoards.h"
#include "MagicBitboards/MagicBitBoardShared.h"


class MagicNumberGenerator {
    std::mt19937_64 rng;
    MBBHelpers mbbHelpers;

public:

    MagicNumberGenerator();

    Bitboard randomBitboard(){ return rng() & rng() & rng(); }

    Bitboard findMagicNumber(int square, bool is_rook);

    void generateMagicNumbers(){
        generateRookMagics();
        generateBishopMagics();
    };
    void generateRookMagics();
    void generateBishopMagics();

};


#endif //MAGICBITBOARDGENERATOR_H
