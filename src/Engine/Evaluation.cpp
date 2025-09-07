//
// Created by jacks on 14/08/2025.
//


#include "Engine/Evaluation.h"
#include <math.h>


float Evaluator::evaluateMove(Move& move){
    const float scoreBefore = evaluate();
    boardManager_->tryMove(move);
    const float scoreAfter = evaluate();
    boardManager_->undoMove();

    const auto result = scoreAfter - scoreBefore;
    // flip the logical scoring to reflect who took the original move
    if (boardManager_->getCurrentTurn() == WHITE) { return -result; }

    return result;
}

float Evaluator::materialScore(){
    int playerToMoveScore = 0;
    int otherPlayerScore = 0;

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);
        const int pieceCount = boardManager_->getBitboards()->countPiece(pieceName);
        auto pieceValue = pieceScoresArray[pieceName];

        if (pieceColours[pieceName] == boardManager_->getCurrentTurn()) {
            playerToMoveScore += pieceValue * pieceCount;
        } else { otherPlayerScore += pieceValue * pieceCount; }
    }
    return playerToMoveScore - otherPlayerScore;
}

float Evaluator::pieceSquareScore(){
    int playerToMoveScore = 0;
    int otherPlayerTurn = 0;

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);

        // where are these pieces
        Bitboard locations = boardManager_->getBitboards()->getOccupancy(pieceName);

        while (locations) {
            const auto location = std::countr_zero(locations); // bottom set bit
            locations &= ~(1ULL << location); // pop the bit

            int lookupIndex = pieceColours[pieceName] == BLACK ? flipBoard(location) : location;

            auto scoresForThisPiece = getPieceScores(pieceTypes[pieceName]);
            auto pieceScore = scoresForThisPiece[lookupIndex];

            if (pieceColours[pieceName] == boardManager_->getCurrentTurn()) { playerToMoveScore += pieceScore; } else {
                otherPlayerTurn += pieceScore;
            }
        }
    }

    return playerToMoveScore - otherPlayerTurn;
}


float Evaluator::evaluate(){
    auto score = materialScore() + pieceSquareScore();

    // centipawns
    return score / (pieceScoresArray[WP] / 100.f);
}



