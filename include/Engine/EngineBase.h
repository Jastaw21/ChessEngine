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

    // Constructor
    explicit EngineBase();

    // Core Engine Interface
    virtual float evaluate() = 0;
    virtual float evaluateMove(Move& move);
    virtual Move search(int depth = 5);

    // Board State Management
    [[nodiscard]] BoardManager *boardManager(){ return &internalBoardManager_; }
    void loadFEN(const std::string& fen);

    // Search Configuration
    virtual int getSearchDepth() const{ return searchDepth_; }
    virtual void setSearchDepth(const int search_depth){ searchDepth_ = search_depth; }

    // UCI Protocol Interface
    virtual void parseUCI(const std::string& uci) override;
    void go(int depth);
    void stop(){ shouldStop = true; }
    void quit(){ shouldQuit = true; }
    static void makeReady();

    // Performance Testing Interface
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(int depth);

protected:

    // Move Generation Interface
    virtual std::vector<Move> generateMoveList();
    virtual std::vector<Move> generateValidMovesFromPosition(
        const Piece& piece, int startSquare) = 0;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) = 0;
    float alphaBeta(int depth, bool isMaximising, float alpha, float beta);

    // Performance Testing Implementation
    virtual PerftResults perft(int depth);
    virtual int simplePerft(int depth);
    virtual std::vector<PerftResults> perftDivide(int depth);

    // Internal State
    BoardManager internalBoardManager_;

private:

    // Engine Control Flags
    bool shouldStop = false;
    bool shouldQuit = false;
    int searchDepth_ = 4;
};


#endif //ENGINEBASE_H