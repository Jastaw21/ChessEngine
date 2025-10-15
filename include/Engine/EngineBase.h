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
#include "SearchHelpers.h"
#include "TranspositionTable.h"

#include "BoardManager/BoardManager.h"


class EngineBase : public ChessPlayer {
public:

    // Constructor
    explicit EngineBase();

    // Core Engine Interface
    SearchResults Search(int depth = 5);
    SearchResults Search(int MaxDepth, int SearchMs);
    virtual std::vector<Move> generateMoveList();

    virtual bool sendCommand(const std::string& command) override;
    virtual std::string readResponse() override;

    // control flags
    void stop(){ shouldQuit_ = true; }
    void quit(){ shouldQuit_ = true; }

    void reset(){
        internalBoardManager_.resetGame();
        lastSearchEvaluations.reset();
        transpositionTable_.clear();
    }

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
    TranspositionTable& getTranspositionTable(){ return transpositionTable_; }
    Evaluator* getEvaluator(){ return &evaluator_; }

    // UCI Protocol Interface
    virtual void parseUCI(const std::string& uci);
    void setFullFen(const std::string& fen){ internalBoardManager_.setFullFen(fen); }

    // Performance Testing Interface
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(int depth);


    void logError(const std::string& error){ searchLogStream << error << std::endl; }

protected:

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

    std::mt19937 rng;
    std::ofstream searchLogStream;

    std::chrono::steady_clock::time_point deadline;

    SearchResults executeSearch(int depth, bool timed = false);
    float alphaBeta(int depth, float alpha, float beta, int ply, std::vector<Move>& pv, bool timed = false,
                    bool nullMoveAllowed = false);


    std::optional<float> evaluateGameState(int depth, int ply, int boardStatus);
    void SortMoves(std::vector<Move>& moves, const Move& ttMove);
    bool performNullMoveReduction(int depth, float beta, int ply, bool timed,
                                  float& evaluatedValue);
    bool getTranspositionTableValue(int depth, Move& ttMove, float& evalResult);
    void storeTranspositionTableEntry(int depth, float bestScore, Move bestMove);
    float performSearchLoop(std::vector<Move>& moves, const int depth, float alpha, const float beta,
                            const int ply, const bool timed, std::vector<Move>& pv
    );

    SearchStatistics currentSearchStats;
    MoveEvaluations lastSearchEvaluations;
};


#endif //ENGINEBASE_H