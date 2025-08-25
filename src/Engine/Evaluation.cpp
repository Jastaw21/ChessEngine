//
// Created by jacks on 14/08/2025.
//


#include "Engine/Evaluation.h"
#include <math.h>

float EvaluatorBase::evaluateMove(Move& move){
    const float scoreBefore = evaluate();
    boardManager_->tryMove(move);
    const float scoreAfter = evaluate();
    boardManager_->undoMove();

    const auto result = scoreAfter - scoreBefore;
    // flip the logical scoring to reflect who took the original move
    if (boardManager_->getCurrentTurn() == WHITE) { return -result; }

    return result;
}

float EvaluatorBase::materialScore(){
    int currentPlayerTurn = 0;
    int otherPlayerTurn = 0;

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);
        const int pieceCount = boardManager_->getBitboards()->countPiece(pieceName);
        const int pieceValue = pieceValues[pieceName];

        if (pieceColours[pieceName] == boardManager_->getCurrentTurn()) {
            currentPlayerTurn += pieceValue * pieceCount;
        } else { otherPlayerTurn += pieceValue * pieceCount; }
    }
    return currentPlayerTurn - otherPlayerTurn;
}

float EvaluatorBase::pieceSquareScore(){ return 0; }


GoodEvaluator::GoodEvaluator(BoardManager* manager) : EvaluatorBase(manager){
    pieceValues = {
                {WP, 100},
                {BP, 100},

                {WR, 500},
                {BR, 500},

                {WN, 310},
                {BN, 310},

                {WB, 320},
                {BB, 320},

                {WK, 1000},
                {BK, 1000},

                {WQ, 900},
                {BQ, 900}
            };

    std::copy(std::begin({
                      0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0,
                      2, 2, 2, 2, 2, 2, 2, 2,
                      3, 3, 3, 3, 3, 3, 3, 3,
                      3, 3, 3, 3, 3, 3, 3, 3,
                      2, 2, 2, 2, 2, 2, 2, 2,
                      0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0
                  }), std::end({0}), pawnScores);

    // Initialize knightScores array
    std::copy(std::begin({
                      -1, 0, 0, 0, 0, 0, 0, -1,
                      -1, 0, 4, 4, 4, 4, 0, -1,
                      0, 2, 4, 4, 4, 4, 2, 0,
                      0, 2, 4, 5, 5, 4, 2, 0,
                      0, 3, 4, 5, 5, 4, 3, 0,
                      0, 2, 4, 4, 4, 4, 2, 0,
                      -1, 0, 4, 4, 4, 4, 0, -1,
                      -1, 0, 0, 0, 0, 0, 0, -1
                  }), std::end({0}), knightScores);
}

float GoodEvaluator::evaluate(){
    auto score = materialScore() + pieceSquareScore();

    const auto gameResult = boardManager_->getGameResult();
    bool wasCheckmate = gameResult & GameResult::CHECKMATE;

    if (!boardManager_->getMoveHistory().empty() && boardManager_->getMoveHistory().top().resultBits &
        MoveResult::CHECK_MATE) { wasCheckmate = true; }

    if (wasCheckmate) { return -INFINITY; }

    // centipawns
    return score / (pieceValues[WP] / 100.f);
}

float GoodEvaluator::kingSafety(){ return EvaluatorBase::kingSafety(); }


BadEvaluator::BadEvaluator(BoardManager* manager) : EvaluatorBase(manager){
    pieceValues = {
                {WP, 99},
                {BP, 99},

                {WR, 1},
                {BR, 1},

                {WN, 1},
                {BN, 1},

                {WB, 1},
                {BB, 1},

                {WK, 1},
                {BK, 1},

                {WQ, 1},
                {BQ, 1}
            };

    std::copy(std::begin({
                      90, 0, 0, 10, 0, 0, 0, 0,
                      0, 0, 10, 0, 0, 10, 0, 0,
                      2, 2, 2, 2, 2, 2, 2, 2,
                      3, 3, 3, 3, 3, 3, 3, 3,
                      3, 3, 3, 3, 3, 3, 3, 3,
                      2, 2, 2, 2, 2, 2, 2, 2,
                      0, 10, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 90, 0, 0, 0, 0
                  }), std::end({0}), pawnScores);

    // Initialize knightScores array
    std::copy(std::begin({
                      -1, 0, 0, 0, 0, 0, 0, -1,
                      -1, 0, 4, 4, 4, 4, 0, -1,
                      90, 2, 4, 4, 4, 4, 2, 90,
                      90, 2, 4, 5, 5, 4, 2, 90,
                      90, 3, 4, 5, 5, 4, 3, 90,
                      90, 2, 4, 4, 4, 4, 2, 90,
                      -1, 0, 4, 4, 4, 4, 0, -1,
                      -1, 0, 0, 0, 0, 0, 0, -1
                  }), std::end({0}), knightScores);
}

float BadEvaluator::evaluate(){
    const auto result = boardManager_->getGameResult();
    bool wasCheckmate = result & GameResult::CHECKMATE;

    if (!boardManager_->getMoveHistory().empty() && boardManager_->getMoveHistory().top().resultBits &
        MoveResult::CHECK_MATE) { wasCheckmate = true; }

    if (wasCheckmate) { return -INFINITY; }

    return 0;
}