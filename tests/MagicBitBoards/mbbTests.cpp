//
// Created by jacks on 11/07/2025.
//


#include  <gtest/gtest.h>

#include "MagicBitboards/MagicBitBoards.h"


TEST(MagicBitboards, BasicTest){
    MagicBitBoards mbb;
    mbb.init();

    Bitboard occupancy = 0x10008;
    printBitboard(occupancy);
    EXPECT_EQ(mbb.getRookAttacks(0,occupancy), 0x1010e);

    occupancy = 0x10040008001000;
    EXPECT_EQ(mbb.getRookAttacks(44,occupancy), 0x10ec1010101000);
    EXPECT_EQ(mbb.getBishopAttacks(43,occupancy), 0x214001422418000);
}

TEST(MagicBitboards, PawnAttackGeneration){
    MagicBitBoards mbb;

    BitBoards boards;
    boards.setFenPositionOnly("rnbqkbnr/p1pppppp/8/Pp6/8/8/1PPPPPPP/RNBQKBNR w KQkq");

    auto pawnMoves = mbb.getMoves(32, WP, boards);
    EXPECT_EQ(pawnMoves, 0x30000000000);
}