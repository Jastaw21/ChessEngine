//
// Created by jacks on 15/07/2025.
//

#include "EngineShared/UCIParsing/Tokeniser.h"

#include <algorithm>
#include <iostream>

Tokeniser::Tokeniser(const std::string& input){ tokenise(input); }


void Tokeniser::tokenise(const std::string& input){
    std::string token;

    for (const auto& c: input) {
        if (c == ' ') {
            // outsource handling
            handleToken(token);
        } else { token += c; }
    }

    // Handle the last token if input doesn't end with space
    if (!token.empty()) { handleToken(token); }
}

void Tokeniser::handleToken(std::string& builtToken){
    // search position
    TokenType type;
    if (builtToken == "position") { type = TokenType::POSITION_CMD; }
    // search move
    else if (builtToken == "move") { type = TokenType::MOVE_CMD; }
    // search uci
    else if (builtToken == "uci") { type = TokenType::UCI; }
    // search go
    else if (builtToken == "go") { type = TokenType::GO; }
    // search quit
    else if (builtToken == "quit") { type = TokenType::QUIT; }
    // search debug
    else if (builtToken == "debug") { type = TokenType::DEBUG; }
    // search isready
    else if (builtToken == "isready") { type = TokenType::ISREADY; }
    // search register
    else if (builtToken == "register") { type = TokenType::REGISTER; }
    // search setoption
    else if (builtToken == "setoption") { type = TokenType::SETOPTION; }
    // search ucinewgame
    else if (builtToken == "ucinewgame") { type = TokenType::UCI_NEW_GAME; }
    // search id
    else if (builtToken == "id") { type = TokenType::ID; }
    // search bestmove
    else if (builtToken == "bestmove") { type = TokenType::BESTMOVE; }
    // search on
    else if (builtToken == "on") { type = TokenType::ON; }
    // search off
    else if (builtToken == "off") { type = TokenType::OFF; }
    //unknown token
    else { type = getUnknownTokenType(builtToken); }

    tokens.emplace_back(type, builtToken);

    builtToken = ""; // empty the token
}

TokenType Tokeniser::getUnknownTokenType(const std::string& token){
    if (isMove(token))
        return TokenType::MOVE_UCI;
    if (isPosition(token))
        return TokenType::POSITION_RESULT;
    if (isIntLiteral(token))
        return TokenType::INT_LITERAL;

    return TokenType::UNKNOWN;
}

bool Tokeniser::isMove(const std::string& token){
    bool isMove = false;

    if (token.size() == 4) { isMove = true; }
    // should be alpha, digit, alpha, digit
    if (!std::isalpha(token[0]) || !std::isalpha(token[2]) || !std::isdigit(token[1]) || !std::isdigit(token[3]))
        isMove = false;

    return isMove;
}


bool Tokeniser::isPosition(const std::string& token){
    // really weak check if it's a position
    int slashCount = 0;
    for (const auto& c: token) { if (c == '/') { slashCount++; } }
    return slashCount == 7;
}

bool Tokeniser::isIntLiteral(const std::string& token){ return std::ranges::all_of(token, ::isdigit); }