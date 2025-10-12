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

inline float percentOf(int numerator, int denominator){ return denominator > 0 ? 100.f * numerator / denominator : 0; }

struct SearchStatistics {
    int searchID = 0;
    int nodesSearched = 0;
    int depth = 0;

    int endGameExits = 0;
    int nullMoveCutoffs = 0;
    int betaCutoffs = 0;

    int ttProbes = 0;
    int ttHits = 0;
    int ttCutoffs = 0;
    int ttStores = 0;


    int firstMoveCutoffs = 0;

    int pvHashHits = 0;


    void print() const{
        double ebf = std::pow(nodesSearched, 1.0 / depth);
        std::cout << "Search ID: " << searchID << std::endl
                << "Depth: " << depth << std::endl
                << "Nodes:" << nodesSearched << std::endl
                << "ebf: " << ebf << std::endl

                << "tt eval cutoffs: " << ttCutoffs << std::endl
                << "Beta cutoffs: " << betaCutoffs << " " << percentOf(betaCutoffs, nodesSearched) << std::endl
                << "Endgame exits: " << endGameExits << std::endl
                << "Null move cutoffs: " << nullMoveCutoffs << std::endl
                << "Remaining Nodes: " << nodesSearched - ttCutoffs - betaCutoffs - endGameExits - nullMoveCutoffs <<
                std::endl

                << "First move cutoffs: " << firstMoveCutoffs << " " << percentOf(firstMoveCutoffs, betaCutoffs) <<
                std::endl

                << "tt hits: " << ttHits << " " << percentOf(ttHits, ttProbes) << std::endl
                << "tt stores: " << ttStores
                << std::endl;
    }

    float firstMovePercent() const{ return betaCutoffs > 0 ? 100.f * firstMoveCutoffs / betaCutoffs : 0; }
    float cutoffPercent() const{ return betaCutoffs > 0 ? 100.f * betaCutoffs / nodesSearched : 0; }
};

struct SearchResults {
    float score;
    Move bestMove;
    std::vector<Move> variation;

    SearchStatistics stats;
};

struct MoveEvaluations {
    std::vector<Move> moves;
    std::vector<float> scores;

    void reset(){
        moves.clear();
        scores.clear();
    }

    float getScore(const Move& move){
        for (size_t i = 0; i < moves.size(); i++) { if (moves[i].toUCI() == move.toUCI()) { return scores[i]; } }
        return 0;
    }

    void print() const{
        for (size_t i = 0; i < moves.size(); i++) { std::cout << moves[i].toUCI() << " " << scores[i] << std::endl; }
    }
};

class EngineBase : public ChessPlayer {
public:

    // Constructor
    explicit EngineBase();

    // Core Engine Interface
    SearchResults Search(int depth = 5);
    SearchResults Search(int MaxDepth, int SearchMs);
    virtual std::vector<Move> generateMoveList();

    Move getBestMove(int depth){ return Search(depth).bestMove; }

    // control flage
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