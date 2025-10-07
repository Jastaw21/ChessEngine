//
// Created by jacks on 16/06/2025.
//

#ifndef TESTENGINE_H
#define TESTENGINE_H
#include <vector>

#include "EngineBase.h"
#include "Evaluation.h"

class MainEngine final : public EngineBase {
public:

    explicit MainEngine() : EngineBase(){}

    virtual std::vector<Move> generateMoveList() override;
    void setFullFen(const std::string& fen);

    virtual bool sendCommand(const std::string& command) override;
    virtual std::string readResponse() override;

private:

    void processPawnPromotion(std::vector<Move>& validMoves, Move& baseMove);
    virtual void
    generateValidMovesFromPosition(const Piece& piece, int startSquare, std::vector<Move>& moveList) override;
    virtual void generateMovesForPiece(const Piece& piece, std::vector<Move>& moveList) override;

public:
};


#endif //TESTENGINE_H