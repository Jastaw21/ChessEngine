//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <filesystem>

#include "ChessPlayer.h"
#include "EngineShared/UCIParsing/UciParser.h"

class CommandHandlerBase {
public:
    virtual void operator()(const UCICommand& cmd) final;
};


struct PerftResults {
    int nodes = 0;
    int captures = 0;
    int enPassant = 0;
    int castling = 0;
    int checks = 0;
    int checkMate = 0;


    std::string fen = "";

    PerftResults operator+(const PerftResults& rhs) const{
        return PerftResults{
                    .nodes = this->nodes + rhs.nodes, .captures = this->captures + rhs.captures,
                    .enPassant = this->enPassant + rhs.enPassant, .castling = this->castling + rhs.castling,
                    .checks = this->checks + rhs.checks, .checkMate = this->checkMate + rhs.checkMate,
                };
    }

    PerftResults &operator+=(const PerftResults& rhs){
        nodes += rhs.nodes;
        captures += rhs.captures;
        enPassant += rhs.enPassant;
        castling += rhs.castling;
        checks += rhs.checks;
        checkMate += rhs.checkMate;
        return *this;
    }

    bool operator==(const PerftResults& rhs) const{
        return nodes == rhs.nodes && captures == rhs.captures && enPassant == rhs.enPassant && castling == rhs.castling
               && fen == rhs.fen && checks == rhs.checks && checkMate == rhs.checkMate;
    }

    bool operator!=(const PerftResults& rhs) const{ return !(*this == rhs); }

    std::string toString() const{
        return "Nodes: " + std::to_string(nodes) + " Captures: " + std::to_string(captures) + " EnPassant: " +
               std::to_string(enPassant) + " Castling: " + std::to_string(castling) + " Checks: " +
               std::to_string(checks) + " CheckMate: " + std::to_string(checkMate);
    }
};

class EngineBase : public ChessPlayer {
public:

    explicit EngineBase(Colours colour = WHITE);

    virtual Move makeMove() override;

    virtual void parseUCI(const std::string& uci);

private:

    UCIParser parser;
};


#endif //ENGINEBASE_H