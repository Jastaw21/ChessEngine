//
// Created by jacks on 16/06/2025.
//

#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <filesystem>
#include <random>

#include "ChessPlayer.h"
#include "BoardManager/BoardManager.h"


#include "EngineShared/PerftResults.h"


class EvaluatorBase {
public:

    virtual ~EvaluatorBase() = default;
    explicit EvaluatorBase(BoardManager* boardManager): pawnScores{}, knightScores{}{ boardManager_ = boardManager; }

    virtual float evaluate(){ return 0; }
    float evaluateMove(Move& move);

    virtual float materialScore(){ return 0; }
    virtual float pieceSquareScore(){ return 0; }

protected:

    BoardManager* boardManager_;

    std::unordered_map<Piece, float> pieceValues;
    int pawnScores[64];
    int knightScores[64];

    float materialScoreWeight = 10;
    float pieceSquareScoreWeight = 2;
};


class EngineBase : public ChessPlayer {
public:

    // Constructor
    explicit EngineBase();
    explicit EngineBase(EvaluatorBase* evaluator) : ChessPlayer(ENGINE), evaluator_(evaluator){};

    // Core Engine Interface
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

    void setEvaluator(EvaluatorBase* evaluator){ evaluator_ = evaluator; }
    EvaluatorBase *getEvaluator() const{ return evaluator_; }

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
    EvaluatorBase* evaluator_ = nullptr;

private:

    // Engine Control Flags
    bool shouldStop = false;
    bool shouldQuit = false;
    int searchDepth_ = 4;

    std::mt19937 rng; // used for randomising the moves if no best move found;
};


#endif //ENGINEBASE_H