//
// Created by jacks on 14/08/2025.
//

#ifndef CHESS_EVALUATION_H
#define CHESS_EVALUATION_H
#include "BoardManager/BoardManager.h"
#include "EvaluationValues.h"

class Evaluator {
public:

    virtual ~Evaluator() = default;
    explicit Evaluator(BoardManager* boardManager) : boardManager_(boardManager){}
    explicit Evaluator() = default;
    virtual void setBoardManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    virtual float evaluate();
    float evaluateMove(Move& move);

    virtual float materialScore();
    virtual float pieceSquareScore();

protected:

    BoardManager* boardManager_ = nullptr;
};


#endif //CHESS_EVALUATION_H