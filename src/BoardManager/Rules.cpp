//
// Created by jacks on 06/07/2025.
//
#include "BoardManager/Rules.h"

Rules::Rules(){
    for (int square = 0; square < 64; square++) {
        Bitboard rankResult = 0ULL;
        buildRankAttacks(square, rankResult);
        rankAttacks.insert({square, rankResult});

        Bitboard fileResult = 0ULL;
        buildFileAttacks(square, fileResult);
        fileAttacks.insert({square, fileResult});

        Bitboard diagResult = 0ULL;
        buildDiagonalAttacks(square, diagResult);
        diagAttacks.insert({square, diagResult});

        Bitboard knightResult = 0ULL;
        buildKnightAttacks(square, knightResult);
        knightAttacks.insert({square, knightResult});

        Bitboard whitePawnPushResult = 0ULL;
        buildWhitePawnPushes(square, whitePawnPushResult);
        whitePawnPushes.insert({square, whitePawnPushResult});

        Bitboard blackPawnPushResult = 0ULL;
        buildBlackPawnPushes(square, blackPawnPushResult);
        blackPawnPushes.insert({square, blackPawnPushResult});

        Bitboard whitePawnAttackResult = 0ULL;
        buildWhitePawnAttacks(square, whitePawnAttackResult);
        whitePawnAttacks.insert({square, whitePawnAttackResult});

        Bitboard blackPawnAttackResult = 0ULL;
        buildBlackPawnAttacks(square, blackPawnAttackResult);
        blackPawnAttacks.insert({square, blackPawnAttackResult});

        Bitboard kingPushResult = 0ULL;
        buildKingMoves(square, kingPushResult);
        kingMoves.insert({square, kingPushResult});
    }
}