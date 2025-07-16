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
    ASSERT_EQ("a8", Fen::rankAndFileToFen(1, 8));
}

TEST(Fen, fenToRankAndFile){

    // bottom left
    int rank, file;
    Fen::FenToRankAndFile("a1", rank, file);
    ASSERT_EQ(1, rank);
    ASSERT_EQ(1, file);

    Fen::FenToRankAndFile("h8", rank, file);
    ASSERT_EQ(8, rank);
    ASSERT_EQ(8, file);
}