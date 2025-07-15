//
// Created by jacks on 15/07/2025.
//

#ifndef TOKENISER_H
#define TOKENISER_H
#include <string>
#include <variant>
#include <vector>

enum class TokenType {
    POSITION_CMD,
    MOVE_CMD,
    GO,
    UCI,
    QUIT,
    UNKNOWN,
    DEBUG,
    ON,
    OFF,
    ISREADY,
    REGISTER,
    SETOPTION,
    ID,
    BESTMOVE,
    UCI_NEW_GAME,
    MOVE_UCI,
    POSITION_RESULT,
    INT_LITERAL,
};

struct Token {
    TokenType type;
    std::string value;
};

class Tokeniser {
public:

    explicit Tokeniser(const std::string& input);
    std::vector<Token> &getTokens(){ return tokens; }

    static bool isMove(const std::string& token);
    static bool isPosition(const std::string& token);
    static bool isIntLiteral(const std::string& token);

private:

    void tokenise(const std::string& input);
    std::vector<Token> tokens;
    void handleToken(std::string& builtToken);
    static TokenType getUnknownTokenType(const std::string& token);
};
#endif //TOKENISER_H
