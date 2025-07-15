//
// Created by jacks on 15/07/2025.
//

#include "EngineShared/UCIParsing/Tokeniser.h"

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
    if (builtToken == "position") { tokens.push_back(TokenType::POSITION_CMD); }
    // search move
    else if (builtToken == "move") { tokens.push_back(TokenType::MOVE_CMD); }
    // search uci
    else if (builtToken == "uci") { tokens.push_back(TokenType::UCI); }
    // search go
    else if (builtToken == "go") { tokens.push_back(TokenType::GO); }
    // search quit
    else if (builtToken == "quit") { tokens.push_back(TokenType::QUIT); }
    // search debug
    else if (builtToken == "debug") { tokens.push_back(TokenType::DEBUG); }
    // search isready
    else if (builtToken == "isready") { tokens.push_back(TokenType::ISREADY); }
    // search register
    else if (builtToken == "register") { tokens.push_back(TokenType::REGISTER); }
    // search setoption
    else if (builtToken == "setoption") { tokens.push_back(TokenType::SETOPTION); }
    // search ucinewgame
    else if (builtToken == "ucinewgame") { tokens.push_back(TokenType::UCI_NEW_GAME); }
    // search id
    else if (builtToken == "id") { tokens.push_back(TokenType::ID); }
    // search bestmove
    else if (builtToken == "bestmove") { tokens.push_back(TokenType::BESTMOVE); }
    // search on
    else if (builtToken == "on") { tokens.push_back(TokenType::ON); }
    // search off
    else if (builtToken == "off") { tokens.push_back(TokenType::OFF); }
    //unknown token
    else { tokens.push_back(getUnknownTokenType(builtToken)); }

    builtToken = ""; // empty the token
}

TokenType Tokeniser::getUnknownTokenType(const std::string& token){
    if (isMove(token))
        return TokenType::MOVE_UCI;
    if (isPosition(token))
        return TokenType::POSITION_RESULT;

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
    int slashCount = 0;
    for (const auto& c: token) { if (c == '/') { slashCount++; } }
    return slashCount == 7;
}