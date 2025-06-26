//
// Created by jacks on 26/06/2025.
//

#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"


TEST(Engine, perft1){

    TestEngine engine(WHITE);

    EXPECT_EQ(engine.runPerftTest(Fen::STARTING_FEN, 1), 20);
    engine.runPerftDivide(Fen::STARTING_FEN, 1);
    EXPECT_EQ(engine.runPerftTest(Fen::STARTING_FEN, 2), 400);
    engine.runPerftDivide(Fen::STARTING_FEN, 2);
    EXPECT_EQ(engine.runPerftTest(Fen::STARTING_FEN, 3), 8902);
    engine.runPerftDivide(Fen::STARTING_FEN, 3);
}

