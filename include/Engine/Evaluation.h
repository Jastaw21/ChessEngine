//
// Created by jacks on 14/08/2025.
//

#ifndef CHESS_EVALUATION_H
#define CHESS_EVALUATION_H
#include "BoardManager/BoardManager.h"

class BoardManager;

class EvaluatorBase {
public:

    virtual ~EvaluatorBase() = default;
    explicit EvaluatorBase(BoardManager* boardManager) : boardManager_(boardManager), pawnScores{}, knightScores{}{}

    virtual float evaluate(){ return 0; }
    float evaluateMove(Move& move);

    virtual float materialScore();
    virtual float pieceSquareScore();
    virtual float kingSafety(){ return 0; }

protected:

    BoardManager* boardManager_;

    std::unordered_map<Piece, float> pieceValues;
    int pawnScores[64];
    int knightScores[64];

    float materialScoreWeight = 10;
    float pieceSquareScoreWeight = 2;
};

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


#endif //CHESS_EVALUATION_H