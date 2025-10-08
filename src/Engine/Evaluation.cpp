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

    const auto currentTurn = boardManager_->getCurrentTurn();

    // precalculate whether each piece is the current turn to avoid colour lookups
    std::array<bool, PIECE_N> pieceIsCurrentTurn;
    for (int precachePiece = 0; precachePiece < PIECE_N; ++precachePiece) {
        // if it's a white piece...
        if (precachePiece == WP || precachePiece == WN || precachePiece == WB || precachePiece == WQ || precachePiece ==
            WR || precachePiece == WK) {
            // ... and the current turn is white - we should flag it as true
            if (currentTurn == WHITE) { pieceIsCurrentTurn[precachePiece] = true; }
            // current turn is black - ignore this one
            else { pieceIsCurrentTurn[precachePiece] = false; }
        }

        // if it's a black piece
        else {
            if (currentTurn == WHITE) {
                // ... and the current turn is white - we should flag it as false
                pieceIsCurrentTurn[precachePiece] = false;
            }
            // current turn is black - check this one
            else { pieceIsCurrentTurn[precachePiece] = true; }
        }
    }

    for (int pieceToTest = 0; pieceToTest < PIECE_N; ++pieceToTest) {
        auto pieceName = static_cast<Piece>(pieceToTest);
        const int pieceCount = boardManager_->getBitboards()->countPiece(pieceName);
        const auto pieceValue = pieceScoresArray[pieceName];

        if (pieceIsCurrentTurn[pieceToTest]) { playerToMoveScore += pieceValue * pieceCount; } else {
            otherPlayerScore += pieceValue * pieceCount;
        }
    }
    return playerToMoveScore - otherPlayerScore;
}

float Evaluator::pieceSquareScore(){
    int playerToMoveScore = 0;
    int otherPlayerTurn = 0;
    const auto currentTurn = boardManager_->getCurrentTurn();

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);
        const bool pieceIsCurrentTurn = pieceColours[pieceName] == currentTurn;

        // where are these pieces
        Bitboard locations = boardManager_->getBitboards()->getOccupancy(pieceName);

        // precache the results of this piece
        auto& scoresForThisPiece = getPieceScores(pieceName);

        while (locations) {
            const auto location = std::countr_zero(locations); // bottom set bit
            locations &= locations - 1; // pop the bit

            int lookupIndex;
            // manual check to avoid looking up colours - black needs the board flipping
            if (pieceName == BP || pieceName == BB ||
                pieceName == BN || pieceName == BQ ||
                pieceName == BR || pieceName == BK) { lookupIndex = flipBoard(location); }

            // must be white - don't need to flip
            else { lookupIndex = location; }

            // now get the square specific score
            const auto pieceScore = scoresForThisPiece[lookupIndex];
            if (pieceIsCurrentTurn) { playerToMoveScore += pieceScore; } else { otherPlayerTurn += pieceScore; }
        }
    }

    return playerToMoveScore - otherPlayerTurn;
}


float Evaluator::evaluate(){
    const auto score = materialScore() + pieceSquareScore();

    // centipawns
    constexpr auto centiPawn = pieceScoresArray[WP] / 100.f;
    return score / centiPawn;
}