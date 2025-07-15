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
    std::variant<std::monostate, std::string, int> value;
};

class Tokeniser {
public:

    explicit Tokeniser(const std::string& input);
    std::vector<TokenType> &getTokens(){ return tokens; }

private:

    void tokenise(const std::string& input);
    std::vector<TokenType> tokens;
    void handleToken(std::string& builtToken);
    TokenType getUnknownTokenType(const std::string& token);

    bool isMove(const std::string& token);
    bool isPosition(const std::string& token);
};
#endif //TOKENISER_H
