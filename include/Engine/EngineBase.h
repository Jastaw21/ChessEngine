//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <filesystem>
#include <fstream>
#include <random>

#include "ChessPlayer.h"
#include "Evaluation.h"
#include "PerftResults.h"
#include "TranspositionTable.h"

#include "BoardManager/BoardManager.h"

struct SearchResults {
    float score;
    Move bestMove;
    int depth;
    std::vector<Move> variation;

    int nodesSearched = 0;
    int hashHits = 0;
};

struct MoveEvaluations {
    std::vector<Move> moves;
    std::vector<float> scores;

    void reset(){
        moves.clear();
        scores.clear();
    }
};

class EngineBase : public ChessPlayer {
public:

    // Constructor
    explicit EngineBase();

    // Core Engine Interface
    SearchResults Search(int depth = 5);
    SearchResults Search(int MaxDepth, int SearchMs);

    Move getBestMove(int depth){ return Search(depth).bestMove; }

    // control flage
    void stop(){ shouldQuit_ = true; }
    void quit(){ shouldQuit_ = true; }
    void reset(){ internalBoardManager_.resetGame(); }
    void go(int depth);
    void go(int depth, int wtime, int btime, int winc = 0, int binc = 0);
    bool shouldQuit() const{ return shouldQuit_; }

    // Board State Management
    [[nodiscard]] BoardManager* boardManager(){ return &internalBoardManager_; }
    void loadFEN(const std::string& fen);

    // Search Configuration
    virtual int getSearchDepth() const{ return searchDepth_; }
    virtual void setSearchDepth(const int search_depth){ searchDepth_ = search_depth; }
    MoveEvaluations& getLastSearchEvaluations(){ return lastSearchEvaluations; }

    // UCI Protocol Interface
    virtual void parseUCI(const std::string& uci);

    // Performance Testing Interface
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(int depth);


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

    int searchDepth_ = 4;

    std::mt19937 rng; // used for randomising the moves if no best move found;
    std::ofstream searchLogStream;

    std::chrono::steady_clock::time_point deadline;

    float alphaBeta(int depth, float alpha, float beta, int ply, std::vector<Move>& pv, bool timed = false);

    int ScoreMove(const Move& move, const Move& ttMove);
    void SortMoves(std::vector<Move>& moves, const Move& ttMove);
    bool evaluateGameState(int depth, int ply, float& value1);

    int currentSearchID = 0;
    int NodesSearched = 0;
    int HashHits = 0;

    MoveEvaluations lastSearchEvaluations;
};


#endif //ENGINEBASE_H