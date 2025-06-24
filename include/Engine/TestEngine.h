//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"


class TestEngine final : public EngineBase {
public:

    static uint64_t getPrelimValidMoves(const Piece& piece, int square);
    explicit TestEngine(Colours colour);
    explicit TestEngine(Colours colour, BoardManager* boardManager);

    virtual float evaluate() override;
    virtual Move search() override;

    virtual Move makeMove() override;

    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    std::vector<Move> generateMoveList(Position& position) const;


    BoardManager* boardManager_ = nullptr;
};


#endif //TESTENGINE_H
