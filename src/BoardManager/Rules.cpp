//
// Created by jacks on 06/07/2025.
//
#include "BoardManager/Rules.h"

Rules::Rules(){
    for (int square = 0; square < 64; square++) {
        uint64_t rankResult = 0ULL;
        buildRankAttacks(square, rankResult);
        rankAttacks.insert({square, rankResult});

        uint64_t fileResult = 0ULL;
        buildFileAttacks(square, fileResult);
        fileAttacks.insert({square, fileResult});

        uint64_t diagResult = 0ULL;
        buildDiagonalAttacks(square, diagResult);
        diagAttacks.insert({square, diagResult});

        uint64_t knightResult = 0ULL;
        buildKnightAttacks(square, knightResult);
        knightAttacks.insert({square, knightResult});

        uint64_t whitePawnPushResult = 0ULL;
        buildWhitePawnPushes(square, whitePawnPushResult);
        whitePawnPushes.insert({square, whitePawnPushResult});

        uint64_t blackPawnPushResult = 0ULL;
        buildBlackPawnPushes(square, blackPawnPushResult);
        blackPawnPushes.insert({square, blackPawnPushResult});

        uint64_t whitePawnAttackResult = 0ULL;
        buildWhitePawnAttacks(square, whitePawnAttackResult);
        whitePawnAttacks.insert({square, whitePawnAttackResult});

        uint64_t blackPawnAttackResult = 0ULL;
        buildBlackPawnAttacks(square, blackPawnAttackResult);
        blackPawnAttacks.insert({square, blackPawnAttackResult});
    }
}