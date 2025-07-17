//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <filesystem>

#include "ChessPlayer.h"
#include "EngineShared/CommandHandlerBase.h"
#include "EngineShared/UCIParsing/UciParser.h"

class PerftResults;

class EngineBase : public ChessPlayer {
public:

    explicit EngineBase(Colours colour = WHITE);
    void parseUCI(const std::string& uci);

    void stop(){ shouldStop = true; };
    void quit(){ shouldQuit = true; };
    void go(const int depth);
    void makeReady();
    virtual float evaluate(BoardManager& mgr) = 0;
    virtual Move search(int depth = 2) = 0;
    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(BoardManager& mgr, int depth);

    virtual std::vector<Move> generateMoveList(BoardManager& mgr);

protected:

    virtual std::vector<Move> generateValidMovesFromPosition(
        BoardManager& mgr, const Piece& piece, int startSquare) = 0;

    virtual std::vector<Move> generateMovesForPiece(BoardManager& mgr, const Piece& piece) = 0;
    virtual PerftResults perft(int depth, BoardManager& boardManager) = 0;
    virtual int simplePerft(int depth, BoardManager& boardManager) = 0;
    virtual std::vector<PerftResults> perftDivide(int depth, BoardManager& boardManager) = 0;

    Move getBestMove();

    UCIParser parser;
    CommandHandlerBase commandHandler;
    BoardManager* boardManager_ = nullptr;

private:

    bool shouldStop = false;
    bool shouldQuit = false;
};


#endif //ENGINEBASE_H