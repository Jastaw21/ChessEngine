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
    static float minMax(BoardManager& mgr, int depth, bool isMaximising);
    Move search() const;

    virtual Move makeMove() override;

    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    static std::vector<Move> generateMoveList(BoardManager& mgr);
    static uint64_t perft(int depth, BoardManager& mgr_);
    static void perftDivide(int depth, BoardManager& mgr_);
    static void runPerftDivide(const std::string& startingFen, int depth);
    static uint64_t runPerftTest(const std::string& Fen, int depth);


    BoardManager* boardManager_ = nullptr;
};


#endif //TESTENGINE_H
