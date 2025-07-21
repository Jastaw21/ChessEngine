//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"


class TestEngine final : public EngineBase {
public:

    virtual float evaluate() override;
    virtual Move search(int depth = 2) override;

    virtual std::vector<Move> generateMoveList() override;

private:

    virtual std::vector<Move> generateValidMovesFromPosition(const Piece& piece, int startSquare) override;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) override;
    float alphaBeta(int depth, bool isMaximising, float alpha, float beta);
    float minMax(int depth, bool isMaximising);

public:
};


#endif //TESTENGINE_H
