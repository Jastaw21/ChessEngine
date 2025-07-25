//
// Created by jacks on 15/07/2025.
//

#ifndef UCIPARSER_H
#define UCIPARSER_H
#include <optional>
#include <string>
#include <variant>

#include <vector>

#include "Tokeniser.h"


struct UCICommand {};

struct StopCommand {};

struct QuitCommand {};

struct IsReadyCommand {};

struct GoCommand {
    std::optional<int> depth;
};

struct PositionCommand {
    bool isStartPos = true;
    std::string fen;
    std::vector<std::string> moves;
};

struct BestMoveCommand {
    std::string move;
};

using Command = std::variant<
    UCICommand,
    GoCommand,
    StopCommand,
    QuitCommand,
    IsReadyCommand,
    PositionCommand,
    BestMoveCommand
>;

class UCIParser {
public:

    std::optional<Command> parse(const std::string& parseTarget);

private:

    int currentToken = 0;
    std::vector<Token> tokens;

    Token peek();
    Token consume();

    std::optional<Command> parsePosition();
    std::optional<Command> parseGo();
    std::optional<Command> parseBestMove();
};


#endif //UCIPARSER_H
