//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"
#include "EngineShared/CommunicatorBase.h"


class TestEngine final : public EngineBase {
public:

    float pieceSquareScore();
    virtual float evaluate() override;

    virtual std::vector<Move> generateMoveList() override;
    void setFullFen(const std::string& fen);
    float materialScore();
    void setCommunicator(TerminalCommunicator* communicator){ communicator_ = communicator; };

private:

    virtual std::vector<Move> generateValidMovesFromPosition(const Piece& piece, int startSquare) override;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) override;

public:
};


#endif //TESTENGINE_H
