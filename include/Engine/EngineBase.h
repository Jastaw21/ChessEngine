//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <filesystem>
#include <random>

#include "ChessPlayer.h"
#include "Evaluation.h"
#include "BoardManager/BoardManager.h"


#include "EngineShared/PerftResults.h"


class EngineBase : public ChessPlayer {
public:

    // Constructor
    explicit EngineBase();

    explicit EngineBase(std::shared_ptr<EvaluatorBase> evaluator) : ChessPlayer(ENGINE),
                                                                    evaluator_(std::move(evaluator)){};

    // Core Engine Interface
    virtual Move search(int depth = 5);

    // Board State Management
    [[nodiscard]] BoardManager *boardManager(){ return &internalBoardManager_; }
    void loadFEN(const std::string& fen);

    // Search Configuration
    virtual int getSearchDepth() const{ return searchDepth_; }
    virtual void setSearchDepth(const int search_depth){ searchDepth_ = search_depth; }

    // UCI Protocol Interface
    virtual void parseUCI(const std::string& uci);
    void go(int depth);
    Move getBestMove(int depth);
    void stop(){ shouldStop = true; }
    void quit(){ shouldQuit_ = true; }
    void reset(){ internalBoardManager_.resetGame(); }
    bool shouldQuit(){ return shouldQuit_; }

    // Performance Testing Interface
    virtual PerftResults runPerftTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth);
    virtual std::vector<PerftResults> runDivideTest(int depth);

    void setEvaluator(std::shared_ptr<EvaluatorBase> evaluator){ evaluator_ = evaluator; }
    std::shared_ptr<EvaluatorBase> getEvaluator() const{ return evaluator_; }

protected:

    // Move Generation Interface
    virtual std::vector<Move> generateMoveList();
    virtual std::vector<Move> generateValidMovesFromPosition(
        const Piece& piece, int startSquare) = 0;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) = 0;

    // Performance Testing Implementation
    virtual PerftResults perft(int depth);
    virtual int simplePerft(int depth);
    virtual std::vector<PerftResults> perftDivide(int depth);
    float negamax(int depth, int ply);

    // Internal State
    BoardManager internalBoardManager_;
    std::shared_ptr<EvaluatorBase> evaluator_;
    bool shouldQuit_ = false;

private:

    // Engine Control Flags
    bool shouldStop = false;

    int searchDepth_ = 4;

    std::mt19937 rng; // used for randomising the moves if no best move found;
};


#endif //ENGINEBASE_H