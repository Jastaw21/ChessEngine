//
// Created by jacks on 26/06/2025.
//

#include <gtest/gtest.h>
#include "Engine/TestEngine.h"
#include "Utility/Fen.h"


TEST(Engine, perft1){
    TestEngine blackEngine(BLACK);
    //EXPECT_EQ(blackEngine.runPerftTest(Fen::STARTING_FEN, 1),20);
    EXPECT_EQ(blackEngine.runPerftTest(Fen::STARTING_FEN, 2).nodes,400);
    EXPECT_EQ(blackEngine.runPerftTest(Fen::STARTING_FEN, 3).nodes,8902);
    EXPECT_EQ(blackEngine.runPerftTest(Fen::STARTING_FEN, 3).captures,34);

    TestEngine whiteEngine(WHITE);
    EXPECT_EQ(whiteEngine.runPerftTest(Fen::STARTING_FEN, 1).nodes,20);
    EXPECT_EQ(whiteEngine.runPerftTest(Fen::STARTING_FEN, 2).nodes,400);

    EXPECT_EQ(whiteEngine.runPerftTest(Fen::STARTING_FEN, 3).nodes,8902);
    EXPECT_EQ(whiteEngine.runPerftTest(Fen::STARTING_FEN, 3).captures,34);
}