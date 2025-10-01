//
// Created by jacks on 15/07/2025.
//

#ifndef TOKENISER_H
#define TOKENISER_H
#include <string>
#include <vector>

enum class TokenType {
    // simple interactions
    UCI, QUIT, ISREADY, UCI_NEW_GAME, STOP,
    // move setting
    POSITION_CMD, START_POS, FEN_BODY, MOVES, MOVE_VALUE, BESTMOVE,

    // go and it's options
    GO, DEPTH,

    // misc engine options
    DEBUG, ON, OFF,
    REGISTER, SETOPTION, ID,

    // literals
    INT_LITERAL, STRING_LITERAL, DASH,

    // parser utilities
    EOF_TOKEN, UNKNOWN,

    // option setting
    SET, SET_OPTION, SET_VALUE,

    // time control
    WTIME, BTIME, WINC, BINC,
};

struct Token {
    TokenType type;
    std::string value;
};

class Tokeniser {
public:

    explicit Tokeniser(const std::string& input);
    std::vector<Token>& getTokens(){ return tokens; }

    static bool isMove(const std::string& token);
    static bool isPosition(const std::string& token);
    static bool isIntLiteral(const std::string& token);
    static bool isStringLiteral(const std::string& token);

private:

    void tokenise(const std::string& input);
    std::vector<Token> tokens;
    void handleToken(std::string& builtToken);
    static TokenType getUnknownTokenType(const std::string& token);
};
#endif //TOKENISER_H