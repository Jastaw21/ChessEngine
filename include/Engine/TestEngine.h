//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"
#include "EngineShared/CommunicatorBase.h"

class GoodEvaluator : public EvaluatorBase {
public:

    explicit GoodEvaluator(BoardManager* manager);

    virtual float evaluate() override;
    virtual float kingSafety() override;

    float simpleEvaluate();
    float simpleMaterialScore();
};

class BadEvaluator : public EvaluatorBase {
public:

    explicit BadEvaluator(BoardManager* manager);

    virtual float evaluate() override;
};

class TestEngine final : public EngineBase {
public:

    TestEngine() = default;
    explicit TestEngine(const std::shared_ptr<EvaluatorBase>& evaluator) : EngineBase(evaluator){}

    virtual std::vector<Move> generateMoveList() override;
    void setFullFen(const std::string& fen);
    void setCommunicator(TerminalCommunicator* communicator){ communicator_ = communicator; }

private:

    virtual std::vector<Move> generateValidMovesFromPosition(const Piece& piece, int startSquare) override;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) override;

public:
};


#endif //TESTENGINE_H