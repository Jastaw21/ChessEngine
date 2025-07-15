//
// Created by jacks on 15/07/2025.
//

#include <gtest/gtest.h>
#include "../include/EngineShared/UCIParsing/Tokeniser.h"


TEST(Tokenisation, BasicTokens){
    EXPECT_TRUE(Tokeniser::isMove("a2a3"));
    auto tokeniser = Tokeniser("position move go uci quit a1a2 1");

    auto& tokens = tokeniser.getTokens();

    EXPECT_EQ(tokens[0].type, TokenType::POSITION_CMD);
    EXPECT_EQ(tokens[1].type, TokenType::MOVE_CMD);
    EXPECT_EQ(tokens[2].type, TokenType::GO);
    EXPECT_EQ(tokens[3].type, TokenType::UCI);
    EXPECT_EQ(tokens[4].type, TokenType::QUIT);
    EXPECT_EQ(tokens[5].type, TokenType::MOVE_UCI);
    EXPECT_EQ(tokens[6].type, TokenType::INT_LITERAL);

    EXPECT_EQ(tokens[0].value, "position");
    EXPECT_EQ(tokens[1].value, "move");
    EXPECT_EQ(tokens[2].value, "go");
    EXPECT_EQ(tokens[3].value, "uci");
    EXPECT_EQ(tokens[4].value, "quit");
    EXPECT_EQ(tokens[5].value, "a1a2");
    EXPECT_EQ(tokens[6].value, "1");

    EXPECT_EQ(tokens.size(), 7);
}