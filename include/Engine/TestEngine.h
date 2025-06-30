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
    float minMax(BoardManager& mgr, int depth, bool isMaximising) const;
    Move search(int depth = 2) const;

    virtual Move makeMove() override;

    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    static std::vector<Move> generateMoveList(BoardManager& mgr);
    PerftResults perft(int depth, BoardManager& mgr_) const;
    PerftResults runPerftTest(const std::string& Fen, int depth) const;


    BoardManager* boardManager_ = nullptr;
};


#endif //TESTENGINE_H
