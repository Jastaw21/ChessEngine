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


    static float evaluate(BoardManager& mgr);
    float minMax(BoardManager& mgr, int depth, bool isMaximising);
    Move search();

    virtual Move makeMove() override;

    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    std::vector<Move> generateMoveList(BoardManager& mgr) const;


    BoardManager* boardManager_ = nullptr;
};


#endif //TESTENGINE_H
