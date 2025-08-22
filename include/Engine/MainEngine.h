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

    MainEngine() = default;
    explicit MainEngine(const std::shared_ptr<EvaluatorBase>& evaluator) : EngineBase(evaluator){}

    virtual std::vector<Move> generateMoveList() override;
    void setFullFen(const std::string& fen);

private:

    void addPromotionMoves(const Move& move);
    void processPawnPromotion(std::vector<Move>& validMoves, Move& baseMove);
    virtual std::vector<Move> generateValidMovesFromPosition(const Piece& piece, int startSquare) override;
    virtual std::vector<Move> generateMovesForPiece(const Piece& piece) override;

public:
};


#endif //TESTENGINE_H