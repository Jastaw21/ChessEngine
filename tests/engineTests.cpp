//
// Created by jacks on 26/06/2025.
//


#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"

TEST(Engine, EngineEvaluatesCorrectly){
    auto whiteEngine = TestEngine(WHITE);

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

    auto blackEngine = TestEngine(BLACK);
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

TEST(Engine, GeneratedMovesIncludeCastling){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto moves = engine.generateMoveList(manager);
    std::vector<std::string> movesToTest;
    for (auto& move: moves) { movesToTest.push_back(move.toUCI()); }
    EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move) { return move == "e1g1"; }));
    EXPECT_TRUE(std::ranges::any_of(movesToTest, [&](const auto& move) { return move == "e1c1"; }));
}

TEST(Engine, MoveGenerationDoesNotChangeState){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    auto move = createMove(WB, "e2a6");
    ASSERT_TRUE(manager.tryMove(move));

    auto board = manager.getBitboards()->toFEN();
    auto moves = engine.generateMoveList(manager);
    auto newBoard = manager.getBitboards()->toFEN();

    ASSERT_EQ(board, newBoard);
}

TEST(Engine, SearchPerformance){
    auto manager = BoardManager();
    manager.getBitboards()->loadFEN(Fen::KIWI_PETE_FEN);
    auto engine = TestEngine(WHITE, &manager);

    EXPECT_TRUE(engine.makeMove().fileFrom >= 1);
}
