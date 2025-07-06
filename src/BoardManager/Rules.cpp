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
    }
}