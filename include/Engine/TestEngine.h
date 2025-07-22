//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"


class TestEngine final : public EngineBase {
public:

    float pieceSquareScore();
    virtual float evaluateMove(Move& move) override;
    virtual float evaluate() override;
    virtual Move search(int depth = 2) override;

    virtual std::vector<Move> generateMoveList() override;
    void setFullFen(const std::string& fen);
    float materialScore();

private:

    virtual std::vector<Move> generateValidMovesFromPosition(const Piece& piece, int startSquare) override;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) override;
    float alphaBeta(int depth, bool isMaximising, float alpha, float beta);
    float minMax(int depth, bool isMaximising);

public:
};


#endif //TESTENGINE_H
