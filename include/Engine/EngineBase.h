//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <atomic>
#include <filesystem>
#include <fstream>
#include <random>

#include "ChessPlayer.h"
#include "Evaluation.h"
#include "TranspositionTable.h"

#include "BoardManager/BoardManager.h"

#include "PerftResults.h"

struct SearchResults {
    float score;
    Move bestMove;
    int depth;
    std::vector<Move> variation;
};


class EngineBase : public ChessPlayer {
public:

    // Constructor
    explicit EngineBase();

    // Core Engine Interface
    virtual Move searchWithTT(int depth = 5);
    Move search(int depth);
    SearchResults searchWithResult(int depth = 3);

    // Board State Management
    [[nodiscard]] BoardManager* boardManager(){ return &internalBoardManager_; }
    void loadFEN(const std::string& fen);

    // Search Configuration
    virtual int getSearchDepth() const{ return searchDepth_; }
    virtual void setSearchDepth(const int search_depth){ searchDepth_ = search_depth; }

    // UCI Protocol Interface
    virtual void parseUCI(const std::string& uci);
    void go(int depth);


    void stop(){ shouldStop = true; }
    void quit(){ shouldQuit_ = true; }
    Move getBestMove(int depth){ return searchWithResult(depth).bestMove; }

    void reset(){
        // spacing comment
        internalBoardManager_.resetGame();
    }

    bool shouldQuit(){ return shouldQuit_; }

    // Performance Testing Interface
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(int depth);


    float alphaBetaWithResultAndTT(int depth, float alpha, float beta, int ply, std::vector<Move>& pv);
    float alphaBetaWithResult(int depth, float alpha, float beta, int ply, std::vector<Move>& pv);


    void logError(const std::string& error){ searchLogStream << error << std::endl; };
    Evaluator* getEvaluator(){ return &evaluator_; }

protected:

    // Move Generation Interface
    virtual std::vector<Move> generateMoveList();
    virtual void generateValidMovesFromPosition(
        const Piece& piece, int startSquare, std::vector<Move>& moveList) = 0;
    virtual void generateMovesForPiece(const Piece& piece, std::vector<Move>& moveList) = 0;

    // Performance Testing Implementation
    virtual PerftResults perft(int depth);
    virtual int simplePerft(int depth);
    virtual std::vector<PerftResults> perftDivide(int depth);


    // Internal State
    BoardManager internalBoardManager_;
    Evaluator evaluator_{};
    bool shouldQuit_ = false;
    TranspositionTable transpositionTable_;

private:

    // Engine Control Flags
    bool shouldStop = false;
    int searchDepth_ = 4;

    std::mt19937 rng; // used for randomising the moves if no best move found;
    std::ofstream searchLogStream;

    std::atomic<bool> StopFlag;

    void startTimer(int ms);
    void stopTimer();
};


#endif //ENGINEBASE_H