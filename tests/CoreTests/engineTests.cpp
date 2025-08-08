//
// Created by jacks on 26/06/2025.
//


#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"

TEST(EngineTests, BasicEvaluation){
    auto engine = TestEngine();
    engine.setEvaluator(new GoodEvaluator(engine.boardManager()));
    engine.setFullFen(Fen::FULL_STARTING_FEN);

    auto result = engine.getEvaluator()->evaluate();
    EXPECT_EQ(result, 0);

    engine.setFullFen("rn6/2pppp1p/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_GT(engine.getEvaluator()->evaluate(), 0);
}


TEST(EngineTests, LikeForLikeSwapStillGood){
    auto engine = TestEngine();
    engine.setEvaluator(new GoodEvaluator(engine.boardManager()));
    engine.setFullFen("r3k2r/p1pp1pb1/bn1qpnp1/2QPN3/1p2P3/2N4p/PPPBBPPP/R3K2R w KQkq");
    auto move = createMove(WQ, "c5d6");

    auto eval = engine.getEvaluator()->evaluateMove(move);

    EXPECT_GT(eval, 0);
}

TEST(EngineTests, ObviousChecksWork){
    auto engine = TestEngine();
    engine.setEvaluator(new GoodEvaluator(engine.boardManager()));

    engine.setFullFen("rnbqkbnr/pppp1ppp/4p3/8/6P1/5P2/PPPPP2P/RNBQKBNR b KQkq - 0 1");

    auto move = createMove(BQ, "d8h4");
    auto result = engine.getEvaluator()->evaluateMove(move);
    EXPECT_EQ(result, -INFINITY);
}