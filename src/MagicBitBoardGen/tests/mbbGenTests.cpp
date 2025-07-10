//
// Created by jacks on 10/07/2025.
//

#include  <gtest/gtest.h>

#include "MagicBitBoardGen/MagicBitboardGenerator.h"

TEST(MaskGen, rookMaskGen){
    MagicBitboardGenerator mbbg;
    int square = 1;
    EXPECT_EQ(mbbg.generateRookMask(square), 0x202020202027c);

    square = 63;
    EXPECT_EQ(mbbg.generateRookMask(square), 0x7e80808080808000);

    square = 27;
    EXPECT_EQ(mbbg.generateRookMask(square), 0x8080876080800);
}

TEST(MaskGen, bishopMaskGen){
    MagicBitboardGenerator mbbg;
    int square = 1;
    EXPECT_EQ(mbbg.generateBishopMask(square), 0x402010080400);

    square = 63;
    EXPECT_EQ(mbbg.generateBishopMask(square), 0x40201008040200);

    square = 27;
    EXPECT_EQ(mbbg.generateBishopMask(square), 0x40221400142200);
}

TEST(AttackGen, rookAttackGen){
    MagicBitboardGenerator mbbg;

    Bitboard occupancy = 0x1010200044101000;

    EXPECT_EQ(mbbg.getRookAttacks(28, occupancy), 0x1010106c100000);

    occupancy = 0x110100001000084;
    EXPECT_EQ(mbbg.getRookAttacks(42, occupancy), 0x4041b0404040404);
}

TEST(AttackGen, bishopAttackGen){
    MagicBitboardGenerator mbbg;

    const Bitboard occupancy = 0x1000000140;
    EXPECT_EQ(mbbg.getBishopAttacks(27, occupancy), 0x1021400142241);
}