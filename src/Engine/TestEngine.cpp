//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"

#include <math.h>

#include "BoardManager/Rules.h"


std::unordered_map<Piece, int> pieceValues = {
            {WP, 10},
            {BP, 10},

            {WR, 55},
            {BR, 55},

            {WN, 30},
            {BN, 30},

            {WB, 32},
            {BB, 32},

            {WK, 990},
            {BK, 990},

            {WQ, 90},
            {BQ, 90}
        };

namespace Weights {
    float MATERIAL_WEIGHT = 10;
    float PIECE_SQUARE_SCORE = 2;
}


std::vector<Move> TestEngine::generateValidMovesFromPosition(const Piece& piece,
                                                             const int startSquare){
    std::vector<Move> validMoves;

    auto possibleMoves2 = internalBoardManager_.getMagicBitBoards()->getMoves(
        startSquare, piece, *internalBoardManager_.getBitboards());

    while (possibleMoves2) {
        const auto endSquare = std::countr_zero(possibleMoves2); // bottom set bit
        possibleMoves2 &= ~(1ULL << endSquare); // pop the bit
        auto candidateMove = Move(piece, startSquare, endSquare);
        if (internalBoardManager_.checkMove(candidateMove))
            validMoves.push_back(candidateMove);
    }
    return validMoves;
}

std::vector<Move> TestEngine::generateMovesForPiece(const Piece& piece){
    std::vector<Move> pieceMoves;

    auto piecePositions = internalBoardManager_.getBitboards()->getBitboard(piece);

    while (piecePositions) {
        const auto startSquare = std::countr_zero(piecePositions); // bottom set bit
        piecePositions &= ~(1ULL << startSquare); // pop the bit
        auto validMoves = generateValidMovesFromPosition(piece, startSquare);
        pieceMoves.insert(pieceMoves.end(), validMoves.begin(), validMoves.end());
    }

    return pieceMoves;
}

std::vector<Move> TestEngine::generateMoveList(){
    std::vector<Move> moves;
    // check each piece we have
    for (const auto& pieceName: filteredPieces[internalBoardManager_.getCurrentTurn()]) {
        auto pieceMoves = generateMovesForPiece(pieceName);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }
    return moves;
}

void TestEngine::setFullFen(const std::string& fen){ internalBoardManager_.setFen(fen); }


float TestEngine::materialScore(){
    int whiteScore = 0;
    int blackScore = 0;

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);
        const int pieceCount = internalBoardManager_.getBitboards()->countPiece(pieceName);
        const int pieceValue = pieceValues[pieceName];

        if (pieceColours[pieceName] == WHITE) { whiteScore += pieceValue * pieceCount; } else {
            blackScore += pieceValue * pieceCount;
        }
    }
    // lerp the score between + 2Q - 2Q
    const float minScore = -2 * pieceValues[WQ];
    const float maxScore = 2 * pieceValues[WQ];
    return MathUtility::map(whiteScore - blackScore, minScore, maxScore, -1, 1);
}

float TestEngine::pieceSquareScore(){ return 0; }

float TestEngine::evaluate(){
    float score = 0;
    const float materialScore_ = materialScore();
    const float pieceSquareScore_ = pieceSquareScore();
    bool wasCheck = internalBoardManager_.opponentKingInCheck();

    float attackKingScore = 0.f;
    // Invert the traditional scoring. If it's now white, it must have been black to move last
    if (wasCheck) { attackKingScore += internalBoardManager_.getCurrentTurn() == WHITE ? -INFINITY : INFINITY; }

    score = materialScore_ * Weights::MATERIAL_WEIGHT;
    score += pieceSquareScore_ * Weights::PIECE_SQUARE_SCORE;
    score += attackKingScore;
    if (wasCheck) {
        auto move = internalBoardManager_.getMoveHistory().top();
        internalBoardManager_.undoMove();
        std::cout << "Check! From position " << internalBoardManager_.getFullFen() << " Move was: " << move.toUCI() <<
                "Score: " << score << "Mat: " << materialScore_ << " PSQ: " << pieceSquareScore_ << "King: " <<
                attackKingScore << std::endl;

        internalBoardManager_.forceMove(move);
    }
    return score;
}

