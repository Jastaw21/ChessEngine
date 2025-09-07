//
// Created by jacks on 26/06/2025.
//


#include <gtest/gtest.h>
#include "Engine/MainEngine.h"
#include "Engine/Evaluation.h"

#include "EngineShared/ProcessChessEngine.h"

#include "Utility/Fen.h"

TEST(EngineTests, BasicEvaluation){
    auto engine = MainEngine();

    engine.setFullFen(Fen::FULL_STARTING_FEN);

    auto result = engine.getEvaluator()->evaluate();
    EXPECT_EQ(result, 0);

    engine.setFullFen("rn6/2pppp1p/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_GT(engine.getEvaluator()->evaluate(), 0);
}

TEST(EngineTests, LikeForLikeSwapStillGood){
    auto engine = MainEngine();
    engine.setFullFen("r3k2r/p1pp1pb1/bn1qpnp1/2QPN3/1p2P3/2N4p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto move = createMove(WQ, "c5d6");

    auto eval = engine.getEvaluator()->evaluateMove(move);

    EXPECT_GT(eval, 0);
}

TEST(EngineTests, ObviousChecksWork){
    auto engine = MainEngine();

    engine.setFullFen("rnbqkbnr/pppp1ppp/4p3/8/6P1/5P2/PPPPP2P/RNBQKBNR b KQkq - 0 1");

    auto bestMove = engine.getBestMove(3);
    EXPECT_EQ(bestMove.toUCI(), "d8h4");

    engine.reset();
    engine.setFullFen("rnbqkbnr/ppppp2p/8/5pp1/4P3/2N5/PPPP1PPP/R1BQKBNR w KQkq - 0 3");
    auto bestMove2 = engine.getBestMove(3);
    EXPECT_EQ(bestMove2.toUCI(), "d1h5");

    engine.reset();
    engine.setFullFen("4QQ1Q/8/8/8/8/8/8/1k2KR2 w - - 6 42");
    auto bestMove3 = engine.searchWithResult(3);

    std::cout << bestMove3.score;

    engine.reset();
    engine.setFullFen("3q1kr1/r7/8/8/4K3/3Q4/B7/q1q3qR b - - 1 46");
    auto bestMove4 = engine.searchWithResult(3);
    EXPECT_EQ(bestMove4.bestMove.toUCI(), "g1g4");

    engine.parseUCI(
        "position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves a2a3 a7a5 b2b3 a5a4 c2c3 a4b3 d1b3 b7b6 d2d3 c7c5 e2e3 e7e5 g2g3 f7f5 b3d5 b8c6 f2f3 b6b5 h2h3 b5b4 a3a4 b4b3 c3c4 b3b2 d3d4 b2a1q e3e4 c5d4 f3f4 d4d3 g3g4 d3d2 b1d2 e5f4 h3h4 f4f3 a4a5 f3f2 e1d1 f2g1q c4c5 f5e4 g4g5 e4e3 h4h5 e3d2 a5a6 d2c1q d1e2 d7d6 c5d6 g7g6 h5g6 h7h5 g5h6 c6b4 a6a7 b4a2 d6d7 c8d7 g6g7 a2c3 e2d3 c3b1 h6h7 b1d2 g7f8Q e8f8 h7g8Q h8g8 f1e2 d2b1 e2d1 b1d2 d1c2 d2b1 c2b1 d7h3 b1a2 h3f1 d3e4");

    bestMove4 = engine.searchWithResult(1);
    EXPECT_EQ(bestMove4.bestMove.toUCI(), "g1g4");

    engine.reset();
    engine.setFullFen("r2q1kr1/P7/8/3Q4/4K3/8/B3b3/q1q3qR w - - 5 42");
    auto bestMove5 = engine.searchWithResult(3);

    EXPECT_EQ(bestMove5.bestMove.toUCI(), "d5f7");

    engine.reset();
    engine.parseUCI(
        "position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves a2a3 a7a5 b2b3 a5a4 c2c3 a4b3 d1b3 b7b6 d2d3 c7c5 e2e3 e7e5 g2g3 f7f5 b3d5 b8c6 f2f3 b6b5 h2h3 b5b4 a3a4 b4b3 c3c4 b3b2 d3d4 b2a1q e3e4 c5d4 f3f4 d4d3 g3g4 d3d2 b1d2 e5f4 h3h4 f4f3 a4a5 f3f2 e1d1 f2g1q c4c5 f5e4 g4g5 e4e3 h4h5 e3d2 a5a6 d2c1q d1e2 d7d6 c5d6 g7g6 h5g6 h7h5 g5h6 c6b4 a6a7 b4a2 d6d7 c8d7 g6g7 a2c3 e2d3 c3b1 h6h7 b1d2 g7f8Q e8f8 h7g8Q h8g8");
    auto result = engine.searchWithResult(2);

    engine.reset();
    engine.setFullFen("r2q1kr1/P7/8/3Q4/8/3K3b/B7/q1q3qR b KQkq - 0 1");
    auto bestMove6 = engine.searchWithResult(2);
    EXPECT_EQ(bestMove6.bestMove.toUCI(), "g1e3");
}

TEST(EngineTests, FindsMateInTwoSteps){
    auto engine = MainEngine();

    engine.setFullFen("6k1/4pp1p/p5p1/1p1q4/4b1N1/P1Q4P/1PP3P1/7K w - - 0 1");

    const auto bestMove = engine.getBestMove(3);
    EXPECT_EQ(bestMove.toUCI(), "g4h6");

    auto result = engine.searchWithResult(3);

    std::cout << result.depth;
}

TEST(EngineTests, FindsMateInOneStep){
    auto engine = MainEngine();

    engine.setFullFen("5k2/4pp1p/p5pN/1p1q4/4b3/P1Q4P/1PP3P1/7K w - - 0 1");

    const auto bestMove = engine.getBestMove(1);
    EXPECT_EQ(bestMove.toUCI(), "c3h8");
}

TEST(EngineTests, GeneratesPromotionMoves){
    auto engine = MainEngine();
    engine.setFullFen("8/P6k/8/8/6r1/8/3B4/5K2 w - - 0 1");

    auto queening = createMove(WP, "a7a8Q");
    auto bishoping = createMove(WP, "a7a8B");
    auto rooking = createMove(WP, "a7a8R");
    auto knighting = createMove(WP, "a7a8N");

    auto engineGeneratedMoves = engine.generateMoveList();

    auto result = std::ranges::any_of(engineGeneratedMoves,
                                      [&](const Move& move) { return move.toUCI().length() == 5; });

    EXPECT_TRUE(result);
}

TEST(EngineTests, FindsQuickestMate){
    auto engine = MainEngine();

    engine.setFullFen("4k3/2ppppp1/2ppppp1/8/8/8/3PPP2/1Q2KQ2 w - - 0 1");

    const auto bestMove = engine.getBestMove(3);
    EXPECT_EQ(bestMove.toUCI(), "b1b8");

    auto bestMoveWithResult = engine.searchWithResult(3);

    std::cout << bestMoveWithResult.bestMove.toUCI();
}

TEST(EngineTests, FindsObviousMatesAfterMoveString){
    auto engine = MainEngine();

    engine.parseUCI(
        "position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves a2a3 a7a5 b2b3 a5a4 c2c3 a4b3 d1b3 b7b6 d2d3 c7c5 e2e3 e7e5 g2g3 f7f5 b3d5 b8c6 f2f3 b6b5 h2h3 a8a3 b1a3 b5b4 d5c6 d7c6 c3b4 c5b4 d3d4 b4a3 d4e5 a3a2 a1a2 f5f4 e3f4 c8h3 g1h3 c6c5 g3g4 c5c4 f1c4 g7g5 c4g8 h8g8 f4g5 g8g5 h3g5 d8g5 c1g5 h7h5 g4h5 f8a3 a2a3 e8d7 f3f4 d7c6 f4f5 c6b5 e5e6 b5b4 f5f6 b4a3 h5h6 a3a2 e6e7 a2a1 f6f7 a1b1 h6h7 b1a1 e7e8Q a1b1 f7f8Q b1a1 h7h8Q a1b1 g5c1 b1c1 h1f1 c1b1 f1g1 b1c1 g1f1 c1b1 f1g1 b1c1 g1f1 c1b1");

    auto moveResult = engine.searchWithResult(3);

    std::cout << moveResult.bestMove.toUCI();
}

TEST(EngineTests, DoesntDoWeirdStuffWithMoveString){
    auto engine = MainEngine();
    engine.parseUCI(
        "position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves a2a3 a7a5 b2b3 a5a4 c2c3 a4b3 d1b3 b7b6 d2d3 c7c5 e2e3 e7e5 g2g3 f7f5 b3d5 b8c6 f2f3 b6b5 h2h3 b5b4 a3a4 b4b3 c3c4 b3b2 d3d4 b2a1q e3e4 c5d4 f3f4 d4d3 g3g4 d3d2 b1d2 e5f4 h3h4 f4f3 a4a5 f3f2 e1d1 f2g1q c4c5 f5e4 g4g5 e4e3 h4h5 e3d2 a5a6 d2c1q d1e2 d7d6 c5d6 g7g6 h5g6 h7h5 g5h6 c6b4 a6a7 b4a2 d6d7 c8d7 g6g7 a2c3");

    auto moveResult = engine.searchWithResult(3);

    std::cout << moveResult.bestMove.toUCI();
}

TEST(EngineTests, DoesntGenerateIllegalMoves){
    auto engine = MainEngine();

    engine.parseUCI(
        "position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves a2a3 a7a5 b2b3 a5a4 c2c3 a4b3 d1b3 b7b6 d2d3 c7c5 e2e3 e7e5 g2g3 f7f5 b3d5 b8c6 f2f3 b6b5 h2h3 b5b4 a3a4 b4b3 c3c4 b3b2 d3d4 b2a1q e3e4 c5d4 f3f4 d4d3 g3g4 d3d2 b1d2 e5f4 h3h4 f4f3 a4a5 f3f2 e1d1 f2g1q c4c5 f5e4 g4g5 e4e3 h4h5 e3d2 a5a6 d2c1q d1e2 d7d6 c5d6 g7g6 h5g6 h7h5 g5h6 c6b4 a6a7 b4a2 d6d7 c8d7 g6g7 a2c3");

    auto moveResult = engine.generateMoveList();

    EXPECT_EQ(moveResult.size(), 2);
}

TEST(EngineTests, CanSetID){
    MainEngine whiteEngine;

    whiteEngine.parseUCI("set id white");
    EXPECT_EQ(whiteEngine.engineID(), "white");
}