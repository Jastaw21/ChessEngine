//
// Created by jacks on 26/06/2025.
//

#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"


TEST(Perft, perft1){
    const TestEngine blackEngine(BLACK);
    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(blackResultDepth1.nodes, 20);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    const TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 1);
    EXPECT_EQ(whiteResultDepth1.nodes, 20);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft2){
    const TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(blackResultDepth1.nodes, 400);
    EXPECT_EQ(blackResultDepth1.captures, 0);

    const TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 2);
    EXPECT_EQ(whiteResultDepth1.nodes, 400);
    EXPECT_EQ(whiteResultDepth1.captures, 0);
}

TEST(Perft, perft3){
    const TestEngine blackEngine(BLACK);

    const auto blackResultDepth1 = blackEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(blackResultDepth1.nodes, 8902);
    EXPECT_EQ(blackResultDepth1.captures, 34);
    EXPECT_EQ(blackResultDepth1.enPassant, 0);

    const TestEngine whiteEngine(WHITE);
    const auto whiteResultDepth1 = whiteEngine.runPerftTest(Fen::STARTING_FEN, 3);
    EXPECT_EQ(whiteResultDepth1.nodes, 8902);
    EXPECT_EQ(whiteResultDepth1.captures, 34);
    EXPECT_EQ(whiteResultDepth1.enPassant, 0);
}

TEST(Engine, EngineEvaluatesCorrectly){
    const auto whiteEngine = TestEngine(WHITE);

    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);

    // at start the engine should evaluate neutral
    EXPECT_EQ(whiteEngine.evaluate(manager), 0);

    // obvious white advantage should evaluate positive
    manager.getBitboards()->loadFEN("2p5/8/8/8/8/8/8/QQRBN3");
    EXPECT_GT(whiteEngine.evaluate(manager), 0);

    // obvious black advantage should evaluate negative
    manager.getBitboards()->loadFEN("rrn5/qr6/8/8/8/8/8/P7");
    EXPECT_LT(whiteEngine.evaluate(manager), 0);

    const auto blackEngine = TestEngine(BLACK);
    manager.getBitboards()->loadFEN(Fen::STARTING_FEN);
    EXPECT_EQ(blackEngine.evaluate(manager), 0);
    manager.getBitboards()->loadFEN("2p5/8/8/8/8/8/8/QQRBN3");
    EXPECT_LT(blackEngine.evaluate(manager), 0);
    manager.getBitboards()->loadFEN("rrn5/qr6/8/8/8/8/8/P7");
    EXPECT_GT(blackEngine.evaluate(manager), 0);
}

TEST(Engine, EnginePicksObviouslyBestMoves){

    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/1p1q1p2/3R4/8/2N5/8");
    auto engine = TestEngine(BLACK, &manager);

    auto firstWhiteMove = createMove(WN, "c2a3");
    ASSERT_TRUE(manager.tryMove(firstWhiteMove));

    EXPECT_EQ(engine.search().toUCI(), "d5d4");

}

TEST(Engine, EngineThinksAheadWhenPickingMoves){

    auto manager = BoardManager();
    manager.getBitboards()->loadFEN("8/8/8/2pq1p2/3R4/8/8/N7");
    auto engine = TestEngine(BLACK, &manager);

    auto firstWhiteMove = createMove(WN, "a1c2");
    ASSERT_TRUE(manager.tryMove(firstWhiteMove));

    EXPECT_EQ(engine.search(2).toUCI(), "c5d4");

}