//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <filesystem>
#include "ChessPlayer.h"
#include "BoardManager/BoardManager.h"


#include "EngineShared/PerftResults.h"

class EngineBase : public ChessPlayer {
public:

    //ctor etc
    explicit EngineBase();

    // testing
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(int depth);

    virtual float evaluateMove(Move& move) = 0;

    // command runners
    void stop(){ shouldStop = true; };
    void quit(){ shouldQuit = true; };
    void go(int depth);
    static void makeReady();

    virtual void parseUCI(const std::string& uci) override;

    // base engine functions
    virtual float evaluate() = 0;
    virtual Move search(int depth = 2) = 0;
    [[nodiscard]] BoardManager *boardManager(){ return &internalBoardManager_; }

    void loadFEN(const std::string& fen);

    virtual int getSearchDepth() const{ return searchDepth_; }
    virtual void setSearchDepth(const int search_depth){ searchDepth_ = search_depth; }

protected:

    virtual std::vector<Move> generateValidMovesFromPosition(
        const Piece& piece, int startSquare) = 0;

    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) = 0;
    virtual PerftResults perft(int depth);
    virtual int simplePerft(int depth);
    virtual std::vector<PerftResults> perftDivide(int depth);
    virtual std::vector<Move> generateMoveList();


    // components
    BoardManager internalBoardManager_;

private:

    // run conditions
    bool shouldStop = false;
    bool shouldQuit = false;

    int searchDepth_ = 3;
};


#endif //ENGINEBASE_H