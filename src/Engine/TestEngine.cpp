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

constexpr int pawnScores[64] =
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3,
            2, 2, 2, 2, 2, 2, 2, 2,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        };


constexpr int knightScores[64] =
        {
            -1, 0, 0, 0, 0, 0, 0, -1,
            -1, 0, 4, 4, 4, 4, 0, -1,
            0, 2, 4, 4, 4, 4, 2, 0,
            0, 2, 4, 5, 5, 4, 2, 0,
            0, 3, 4, 5, 5, 4, 3, 0,
            0, 2, 4, 4, 4, 4, 2, 0,
            -1, 0, 4, 4, 4, 4, 0, -1,
            -1, 0, 0, 0, 0, 0, 0, -1,
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

void TestEngine::setFullFen(const std::string& fen){ internalBoardManager_.setFullFen(fen); }


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

float TestEngine::pieceSquareScore(){
    float whitePieceSquareScore = 0;
    float blackPieceSquareScore = 0;

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);

        Bitboard pieceSquares = internalBoardManager_.getBitboards()->getBitboard(pieceName);
        while (pieceSquares) {
            const int square = std::countr_zero(pieceSquares);
            pieceSquares &= ~(1ULL << square);

            switch (pieceName) {
                case BP:
                    blackPieceSquareScore += pawnScores[square];
                case WP:
                    whitePieceSquareScore += pawnScores[square];

                case WN:
                    whitePieceSquareScore += knightScores[square];
                case BN:
                    blackPieceSquareScore += knightScores[square];

                default:
                    break;
            }
        }
    }

    return MathUtility::map(whitePieceSquareScore - blackPieceSquareScore, -pieceValues[WQ], pieceValues[WQ],
                            -pieceValues[WP], pieceValues[WP]);
}

float TestEngine::evaluate(){
    float score = 0;
    const float materialScore_ = materialScore();
    const float pieceSquareScore_ = pieceSquareScore();
    const auto result = internalBoardManager_.getGameResult();
    const bool wasCheck = result & CHECK;
    const bool wasCheckmate = result & CHECKMATE;

    float attackKingScore = 0.f;
    // Invert the traditional scoring. If it's now white, it must have been black to move last
    if (wasCheck) { std::cout << "Check" << std::endl; }
    if (wasCheckmate) {
        std::cout << "Checkmate" << std::endl;
        attackKingScore += internalBoardManager_.getCurrentTurn() == WHITE ? -INFINITY : INFINITY;
    }

    score = materialScore_ * Weights::MATERIAL_WEIGHT;
    score += pieceSquareScore_ * Weights::PIECE_SQUARE_SCORE;
    score += attackKingScore;

    return score;
}

