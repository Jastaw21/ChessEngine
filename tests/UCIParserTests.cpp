//
// Created by jacks on 15/07/2025.
//

#include <gtest/gtest.h>
#include "../include/EngineShared/UCIParsing/Tokeniser.h"
#include "EngineShared/UCIParsing/UciParser.h"


TEST(Tokenisation, BasicTokens){
    EXPECT_TRUE(Tokeniser::isMove("a2a3"));
    auto tokeniser = Tokeniser("position moves go uci quit a1a2 1");

    auto& tokens = tokeniser.getTokens();

    EXPECT_EQ(tokens[0].type, TokenType::POSITION_CMD);
    EXPECT_EQ(tokens[1].type, TokenType::MOVES);
    EXPECT_EQ(tokens[2].type, TokenType::GO);
    EXPECT_EQ(tokens[3].type, TokenType::UCI);
    EXPECT_EQ(tokens[4].type, TokenType::QUIT);
    EXPECT_EQ(tokens[5].type, TokenType::MOVE_VALUE);
    EXPECT_EQ(tokens[6].type, TokenType::INT_LITERAL);
    EXPECT_EQ(tokens[7].type, TokenType::EOF_TOKEN);

    EXPECT_EQ(tokens[0].value, "position");
    EXPECT_EQ(tokens[1].value, "moves");
    EXPECT_EQ(tokens[2].value, "go");
    EXPECT_EQ(tokens[3].value, "uci");
    EXPECT_EQ(tokens[4].value, "quit");
    EXPECT_EQ(tokens[5].value, "a1a2");
    EXPECT_EQ(tokens[6].value, "1");
    EXPECT_EQ(tokens[7].value, "");

    EXPECT_EQ(tokens.size(), 8);
}

TEST(Parsing, UCICommand){
    auto parser = UCIParser{};

    auto result = parser.parse("uci");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<UCICommand>(*result));

    // works even with muck at the end
    result = parser.parse("uci blah blah");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<UCICommand>(*result));
}

TEST(Parsing, StopCommandWorks){
    auto parser = UCIParser{};
    const auto result = parser.parse("stop");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<StopCommand>(*result));

    const auto result2 = parser.parse("stop blah blah");
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(std::holds_alternative<StopCommand>(*result2));
}

TEST(Parsing, QuitCommandWorks){
    auto parser = UCIParser{};
    const auto result = parser.parse("quit");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<QuitCommand>(*result));

    const auto result2 = parser.parse("quit blah blah");
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(std::holds_alternative<QuitCommand>(*result2));
}

TEST(Parsing, IsReadyCommandWorks){
    auto parser = UCIParser{};
    const auto result = parser.parse("isready");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<IsReadyCommand>(*result));

    const auto result2 = parser.parse("isready blah blah");
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(std::holds_alternative<IsReadyCommand>(*result2));
}

TEST(Parsing, PositionWorksWithStartPos){
    auto parser = UCIParser{};
    const auto result = parser.parse("position startpos");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<PositionCommand>(*result));
    EXPECT_TRUE(std::get<PositionCommand>(*result).isStartPos);
    EXPECT_EQ(std::get<PositionCommand>(*result).fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    const auto result2 = parser.parse("position startpos");
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(std::holds_alternative<PositionCommand>(*result2));
    EXPECT_TRUE(std::get<PositionCommand>(*result2).isStartPos);
    EXPECT_EQ(std::get<PositionCommand>(*result2).fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

TEST(Parsing, BasicGoCommandParsing){
    auto parser = UCIParser{};
    const auto result = parser.parse("go");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<GoCommand>(*result));
    EXPECT_FALSE(std::get<GoCommand>(*result).depth.has_value());

    const auto result2 = parser.parse("go blah blah");
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(std::holds_alternative<GoCommand>(*result2));
    EXPECT_FALSE(std::get<GoCommand>(*result2).depth.has_value());
}

TEST(Parsing, GoWithDepth){
    auto parser = UCIParser{};
    const auto result = parser.parse("go depth 1");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<GoCommand>(*result));
    EXPECT_EQ(std::get<GoCommand>(*result).depth, 1);
}

TEST(Parsing, BestMoveCommandDoesntWorkWithoutMove){
    auto parser = UCIParser{};
    const auto result = parser.parse("bestmove");
    ASSERT_FALSE(result.has_value());
}

TEST(Parsing, BestMoveCommandWorks){
    auto parser = UCIParser{};
    const auto result = parser.parse("bestmove a1a2");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(std::holds_alternative<BestMoveCommand>(*result));
    EXPECT_EQ(std::get<BestMoveCommand>(*result).move, "a1a2");
}