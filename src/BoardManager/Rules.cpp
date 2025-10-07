//
// Created by jacks on 06/07/2025.
//
#include "BoardManager/Rules.h"

Rules::Rules(){
    for (int square = 0; square < 64; square++) {
        Bitboard rankResult = 0ULL;
        buildRankAttacks(square, rankResult);
        rankAttacks[square] = rankResult;

        Bitboard fileResult = 0ULL;
        buildFileAttacks(square, fileResult);
        fileAttacks[square] = fileResult;

        Bitboard diagResult = 0ULL;
        buildDiagonalAttacks(square, diagResult);
        diagAttacks[square] = diagResult;

        Bitboard knightResult = 0ULL;
        buildKnightAttacks(square, knightResult);
        knightAttacks[square] = knightResult;

        Bitboard whitePawnPushResult = 0ULL;
        buildWhitePawnPushes(square, whitePawnPushResult);
        whitePawnPushes[square] = whitePawnPushResult;

        Bitboard blackPawnPushResult = 0ULL;
        buildBlackPawnPushes(square, blackPawnPushResult);
        blackPawnPushes[square] = blackPawnPushResult;

        Bitboard whitePawnAttackResult = 0ULL;
        buildWhitePawnAttacks(square, whitePawnAttackResult);
        whitePawnAttacks[square] = whitePawnAttackResult;

        Bitboard blackPawnAttackResult = 0ULL;
        buildBlackPawnAttacks(square, blackPawnAttackResult);
        blackPawnAttacks[square] = blackPawnAttackResult;

        Bitboard kingPushResult = 0ULL;
        buildKingMoves(square, kingPushResult);
        kingMoves[square] = kingPushResult;
    }
}

