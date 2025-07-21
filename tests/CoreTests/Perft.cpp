//
// Created by jacks on 14/07/2025.
//

#include <gtest/gtest.h>
#include "perftTestUtility.h"
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"

TEST(Divide, kiwiPeteDivide){
    constexpr int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(Divide, kiwiPeteDivide2){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(Divide, kiwiPeteDivide3){
    constexpr int depth = 3;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::KIWI_PETE_FEN, outputFile, depth));
}

TEST(Divide, perft1Divide){
    const int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(Divide, perft2Divide){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(Divide, perft4Divide){
    constexpr int depth = 4;
    // get the moves the actual engine think are possible
    const std::string outputFile = "startPos.txt";
    EXPECT_TRUE(divideTest(Fen::STARTING_FEN, outputFile, depth));
}

TEST(Divide, position3Divide){
    constexpr int depth = 1;
    // get the moves the actual engine think are possible
    const std::string outputFile = "pos3.txt";
    EXPECT_TRUE(divideTest(Fen::POSITION_3_FEN, outputFile, depth));
}

TEST(Divide, position3Divide2){
    constexpr int depth = 2;
    // get the moves the actual engine think are possible
    const std::string outputFile = "pos3.txt";
    EXPECT_TRUE(divideTest(Fen::POSITION_3_FEN, outputFile, depth));
}

TEST(Perft, perft1){
    TestEngine blackEngine;
    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 20);
    EXPECT_EQ(blackResultDepth1.captures, 0);
}

TEST(Perft, perft2){
    TestEngine whiteEngine;
    auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(whiteResultDepth1.nodes, 400);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft3){
    TestEngine whiteEngine;
    auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(whiteResultDepth1.nodes, 8902);
    EXPECT_EQ(whiteResultDepth1.captures, 34);
    EXPECT_EQ(whiteResultDepth1.enPassant, 0);
    EXPECT_EQ(whiteResultDepth1.checks, 12);
}

TEST(Perft, perft4){
    TestEngine blackEngine;

    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 4);
    EXPECT_EQ(blackResultDepth1.nodes, 197281);
    EXPECT_EQ(blackResultDepth1.captures, 1576);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);
    EXPECT_EQ(blackResultDepth1.castling, 0);
    EXPECT_EQ(blackResultDepth1.checks, 469);
    EXPECT_EQ(blackResultDepth1.checkMate, 8);
}

TEST(Perft, kiwiPete1){
    TestEngine blackEngine;
    auto blackResultDepth1 = blackEngine.runPerftTest(Fen::KIWI_PETE_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 48);
    EXPECT_EQ(blackResultDepth1.captures, 8);
    EXPECT_EQ(blackResultDepth1.castling, 2);
}

TEST(Perft, kiwiPete2){
    TestEngine whiteEngine;
    auto perftResults = whiteEngine.runPerftTest(Fen::KIWI_PETE_FEN, 2);
    EXPECT_EQ(perftResults.nodes, 2039);
    EXPECT_EQ(perftResults.captures, 351);
    EXPECT_EQ(perftResults.castling, 91);
    EXPECT_EQ(perftResults.enPassant, 1);
    EXPECT_EQ(perftResults.checks, 3);
}

TEST(Perft, kiwiPete3){
    TestEngine whiteEngine;
    auto perftResults = whiteEngine.runPerftTest(Fen::KIWI_PETE_FEN, 3);
    EXPECT_EQ(perftResults.nodes, 97862);
    EXPECT_EQ(perftResults.captures, 17102);
    EXPECT_EQ(perftResults.castling, 3162);
    EXPECT_EQ(perftResults.enPassant, 45);
    EXPECT_EQ(perftResults.checks, 993);
    EXPECT_EQ(perftResults.checkMate, 1);
}

TEST(Perft, position3Depth1){
    TestEngine engine;
    auto perftResults = engine.runPerftTest(Fen::POSITION_3_FEN, 1);
    EXPECT_EQ(perftResults.nodes, 14);
    EXPECT_EQ(perftResults.captures, 1);
    EXPECT_EQ(perftResults.checks, 2);
}

TEST(Perft, position3Depth2){
    TestEngine engine;
    auto perftResults = engine.runPerftTest(Fen::POSITION_3_FEN, 2);
    EXPECT_EQ(perftResults.nodes, 191);
    EXPECT_EQ(perftResults.captures, 14);
    EXPECT_EQ(perftResults.checks, 10);
}

