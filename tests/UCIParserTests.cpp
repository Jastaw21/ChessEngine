//
// Created by jacks on 15/07/2025.
//

#include <gtest/gtest.h>
#include "../include/EngineShared/UCIParsing/Tokeniser.h"


TEST(Tokenisation, BasicTokens){
    auto tokeniser = Tokeniser("position move go uci quit a1a2");

    auto& tokens = tokeniser.getTokens();

    EXPECT_EQ(tokens[0], TokenType::POSITION_CMD);
    EXPECT_EQ(tokens[1], TokenType::MOVE_CMD);
    EXPECT_EQ(tokens[2], TokenType::GO);
    EXPECT_EQ(tokens[3], TokenType::UCI);
    EXPECT_EQ(tokens[4], TokenType::QUIT);
    EXPECT_EQ(tokens[5], TokenType::MOVE_UCI);

    EXPECT_EQ(tokens.size(), 6);
}