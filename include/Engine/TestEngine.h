//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"


class TestEngine final : public EngineBase {
public:

    explicit TestEngine(Colours colour);
    explicit TestEngine(Colours colour, BoardManager* boardManager);

    float evaluate(BoardManager& mgr) const;
    virtual Move search(int depth = 2) override;

    virtual std::vector<Move> generateMoveList(BoardManager& mgr) override;

private:

    virtual std::vector<Move>
    generateValidMovesFromPosition(BoardManager& mgr, const Piece& piece, int startSquare) override;
    virtual std::vector<Move> generateMovesForPiece(BoardManager& mgr, const Piece& piece) override;
    float minMax(BoardManager& mgr, int depth, bool isMaximising);

public:

    virtual float evaluate(BoardManager& mgr) override;
};


#endif //TESTENGINE_H
