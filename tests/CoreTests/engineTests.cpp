//
// Created by jacks on 26/06/2025.
//


#include <gtest/gtest.h>
#include "Engine/MainEngine.h"
#include "Engine/Evaluation.h"
#include "Utility/Fen.h"

TEST(EngineTests, BasicEvaluation){
    auto engine = MainEngine();
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));
    engine.setFullFen(Fen::FULL_STARTING_FEN);

    auto result = engine.getEvaluator()->evaluate();
    EXPECT_EQ(result, 0);

    engine.setFullFen("rn6/2pppp1p/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_GT(engine.getEvaluator()->evaluate(), 0);
}


TEST(EngineTests, LikeForLikeSwapStillGood){
    auto engine = MainEngine();
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));
    engine.setFullFen("r3k2r/p1pp1pb1/bn1qpnp1/2QPN3/1p2P3/2N4p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto move = createMove(WQ, "c5d6");

    auto eval = engine.getEvaluator()->evaluateMove(move);

    EXPECT_GT(eval, 0);
}

TEST(EngineTests, ObviousChecksWork){
    auto engine = MainEngine();
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));

    engine.setFullFen("rnbqkbnr/pppp1ppp/4p3/8/6P1/5P2/PPPPP2P/RNBQKBNR b KQkq - 0 1");

    auto bestMove = engine.getBestMove(3);
    EXPECT_EQ(bestMove.toUCI(), "d8h4");

    engine.reset();
    engine.setFullFen("rnbqkbnr/ppppp2p/8/5pp1/4P3/2N5/PPPP1PPP/R1BQKBNR w KQkq - 0 3");
    auto bestMove2 = engine.getBestMove(3);
    EXPECT_EQ(bestMove2.toUCI(), "d1h5");
}

TEST(EngineTests, FindsMateInTwoSteps){
    auto engine = MainEngine();
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));
    engine.setFullFen("6k1/4pp1p/p5p1/1p1q4/4b1N1/P1Q4P/1PP3P1/7K w - - 0 1");

    const auto bestMove = engine.getBestMove(3);
    EXPECT_EQ(bestMove.toUCI(), "g4h6");

    auto result = engine.searchWithResult(3);

    std::cout << result.depth;
}

TEST(EngineTests, FindsMateInOneStep){
    auto engine = MainEngine();
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));
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
    engine.setEvaluator(std::make_shared<GoodEvaluator>(engine.boardManager()));
    engine.setFullFen("4k3/2ppppp1/2ppppp1/8/8/8/3PPP2/1Q2KQ2 w - - 0 1");

    const auto bestMove = engine.getBestMove(3);
    EXPECT_EQ(bestMove.toUCI(), "b1b8");

    auto bestMoveWithResult = engine.searchWithResult(3);

    std::cout << bestMoveWithResult.bestMove.toUCI();
}