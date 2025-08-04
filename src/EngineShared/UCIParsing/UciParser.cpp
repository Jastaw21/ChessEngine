#include "../../../include/EngineShared/UCIParsing/UciParser.h"

#include <iostream>
#include <string>

#include "EngineShared/UCIParsing/Tokeniser.h"


std::optional<Command> UCIParser::parse(const std::string& inString){
    currentToken = 0;
    auto tokeniser = Tokeniser(inString);
    tokens = tokeniser.getTokens();

    while (currentToken < tokens.size()) {
        const Token liveToken = consume();

        if (liveToken.type == TokenType::UNKNOWN) { break; }

        if (liveToken.type == TokenType::UCI) { return UCICommand{}; }

        if (liveToken.type == TokenType::STOP) { return StopCommand{}; }

        if (liveToken.type == TokenType::QUIT) { return QuitCommand{}; }

        if (liveToken.type == TokenType::ISREADY) { return IsReadyCommand{}; }

        if (liveToken.type == TokenType::UCI_NEW_GAME) { return NewGameCommand{}; }

        if (liveToken.type == TokenType::GO) { return parseGo(); }

        if (liveToken.type == TokenType::POSITION_CMD) { return parsePosition(); }

        if (liveToken.type == TokenType::BESTMOVE) { return parseBestMove(); }
    }

    return std::nullopt; // no valid command found
}


Token UCIParser::peek(){
    if (currentToken < tokens.size()) { return tokens.at(currentToken); }
    return Token{.type = TokenType::EOF_TOKEN, .value = ""};
}

Token UCIParser::consume(){
    Token returnToken = peek();
    currentToken++;
    return returnToken;
}

std::optional<Command> UCIParser::parsePosition(){
    // after getting the position command, the next token can be startpos or a fen
    PositionCommand result;
    if (peek().type == TokenType::START_POS) {
        result.isStartPos = true;
        result.fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        consume();
    }
    // if we don't get a startpos, means we need to try and parse the fen
    else if (peek().type == TokenType::FEN_BODY) {
        std::string fenBody = peek().value;
        consume();
        result.isStartPos = false;
        result.fen = fenBody;

        while (peek().type == TokenType::STRING_LITERAL
               || peek().type == TokenType::INT_LITERAL
               || peek().type == TokenType::DASH
        ) { result.fen += ' ' + consume().value; }
    }

    if (peek().type == TokenType::MOVES) {
        consume();
        while (peek().type == TokenType::MOVE_VALUE) { result.moves.emplace_back(consume().value); }
    }

    return result;
}

std::optional<Command> UCIParser::parseGo(){
    GoCommand result;
    result.depth = std::nullopt; // default to null depth, handle that in the engine
    if (peek().type == TokenType::DEPTH) {
        consume();
        if (peek().type == TokenType::INT_LITERAL) { result.depth = std::min(std::stoi(consume().value), 10); }
    }
    return result;
}

std::optional<Command> UCIParser::parseBestMove(){
    if (peek().type == TokenType::MOVE_VALUE) { return BestMoveCommand{consume().value}; }
    return std::nullopt;
}


