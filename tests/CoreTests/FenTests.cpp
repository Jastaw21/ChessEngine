//
// Created by jacks on 21/06/2025.
//


#include  <gtest/gtest.h>

#include "Utility/Fen.h"


TEST(Fen, rankAndFileToFen){
    // bottom left
    ASSERT_EQ("a1", Fen::rankAndFileToFen(1, 1));

    //top right
    ASSERT_EQ("h8", Fen::rankAndFileToFen(8, 8));

    // top left
    ASSERT_EQ("a8", Fen::rankAndFileToFen(8, 1));
}

TEST(Fen, fenToRankAndFile){
    // bottom left
    int rank;
    int file;
    Fen::FenToRankAndFile("a1", rank, file);
    ASSERT_EQ(1, rank);
    ASSERT_EQ(1, file);

    Fen::FenToRankAndFile("h8", rank, file);
    ASSERT_EQ(8, rank);
    ASSERT_EQ(8, file);
}

TEST(Fen, FenToSquare){
    // bottom left
    ASSERT_EQ(0, Fen::FenToSquare("a1"));

    // top right
    ASSERT_EQ(63, Fen::FenToSquare("h8"));

    // top left
    ASSERT_EQ(56, Fen::FenToSquare("a8"));

    // middle
    ASSERT_EQ(32, Fen::FenToSquare("a5"));
}

TEST(Fen, SquareToFen){
    EXPECT_EQ("a1", Fen::squareToFen(0));
    EXPECT_EQ("h8", Fen::squareToFen(63));
    EXPECT_EQ("a8", Fen::squareToFen(56));
    EXPECT_EQ("a5", Fen::squareToFen(32));
}