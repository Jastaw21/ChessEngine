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
    Move search(int depth = 2);
    virtual Move makeMove() override;

    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    static std::vector<Move> generateMoveList(BoardManager& mgr);

    PerftResults runPerftTest(const std::string& Fen, int depth) const;
    std::vector<PerftResults> runDivideTest(const std::string& Fen, int depth) const;
    static std::vector<PerftResults> runDivideTest(BoardManager& mgr, int depth);


    BoardManager* boardManager_ = nullptr;

private:

    static std::vector<Move> generateValidMovesFromPosition(BoardManager& mgr, const Piece& piece, int startSquare);
    static std::vector<Move> generateMovesForPiece(BoardManager& mgr, const Piece& piece);
    float minMax(BoardManager& mgr, int depth, bool isMaximising);
    static PerftResults perft(int depth, BoardManager& boardManager);
    static auto simplePerft(int depth, BoardManager& boardManager);
    static auto perftDivide(int depth, BoardManager& boardManager);
};


#endif //TESTENGINE_H
