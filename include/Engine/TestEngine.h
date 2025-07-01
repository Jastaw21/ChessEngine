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
    std::vector<Move> generateValidMovesFromPosition(const BoardManager& mgr, Piece piece, int start_square) const;
    std::vector<Move> generateMovesForPiece(BoardManager& mgr, const Piece& piece) const;

    void setManager(BoardManager* boardManager){ boardManager_ = boardManager; }

    static std::vector<Move> generateMoveList(BoardManager& mgr);
    PerftResults perft(int depth, BoardManager& boardManager) const;
    PerftResults runPerftTest(const std::string& Fen, int depth) const;


    BoardManager* boardManager_ = nullptr;
};


#endif //TESTENGINE_H
