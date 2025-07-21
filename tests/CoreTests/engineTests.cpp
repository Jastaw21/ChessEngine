//
// Created by jacks on 26/06/2025.
//


#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"

TEST(EngineTests, BasicEvaluation){
    auto engine = TestEngine();
    engine.loadFEN(Fen::STARTING_FEN);

    auto result = engine.evaluate();
    EXPECT_EQ(result, 0);

    engine.loadFEN("rn6/2pppp1p/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_GT(engine.evaluate(), 0);
}

TEST(AmundsenPostionTests, Basics){
    auto engine = TestEngine();
    engine.loadFEN("r1bq1r1k/p1pnbpp1/1p2p3/6p1/3PB3/5N2/PPPQ1PPP/2KR3R w - - ");

    for (int depth = 1; depth < 5; depth++) {
        auto result = engine.search(depth);
        std::cout << result.toUCI() << std::endl;
    }
}

