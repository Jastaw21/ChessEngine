//
// Created by jacks on 22/07/2025.
//

#include  <gtest/gtest.h>

#include "BoardManager/BoardManager.h"
#include "Engine/Piece.h"
#include "Utility/math.h"

TEST(Math, Utility){
    int i = 0;
    EXPECT_EQ(0, MathUtility::sign(i));
    int j = 99;
    EXPECT_EQ(1, MathUtility::sign(j));
    int k = -99;
    EXPECT_EQ(-1, MathUtility::sign(k));
}

TEST(Math, Lerp){
    float min = -10;
    float max = 10;
    float value = MathUtility::lerp(0.f, min, max);
    EXPECT_EQ(0.5f, value);

    EXPECT_EQ(1.f, MathUtility::lerp(15.f, min, max));
    EXPECT_EQ(-1.f, MathUtility::lerp(-15.f, min, max));
}

TEST(Math, Map){
    float min = -1;
    float max = 1;
    float value = MathUtility::map(0.f, min, max, 0, 12);
    EXPECT_EQ(6, value);

    EXPECT_EQ(0.5, MathUtility::map(1.f, 0, 2, 0, 1));
    EXPECT_EQ(0, MathUtility::map(1.f, 0, 2, -1, 1));
}

TEST(MoveUtility, InverseOf){
    auto move = createMove(WP, "a1a2");
    auto move2 = createMove(WP, "a2a1");
    EXPECT_TRUE(move.isInverseOf(move2));

    auto move3 = createMove(WP, "a1a3");
    EXPECT_FALSE(move.isInverseOf(move3));
    EXPECT_FALSE(move2.isInverseOf(move3));
    EXPECT_FALSE(move3.isInverseOf(move));
    EXPECT_FALSE(move3.isInverseOf(move2));
}