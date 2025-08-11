//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"

#include <math.h>

#include "BoardManager/Rules.h"


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
    float score = 0;
    const float materialScore_ = materialScore();
    const float pieceSquareScore_ = pieceSquareScore();
    const auto result = boardManager_->getGameResult();
    bool wasCheckmate = result & GameResult::CHECKMATE;

    if (boardManager_->getMoveHistory().size() > 0 && boardManager_->getMoveHistory().top().resultBits &
        MoveResult::CHECK_MATE) { wasCheckmate = true; }

    float attackKingScore = 0.f;
    // Invert the traditional scoring. If it's now white, it must have been black to move last

    if (wasCheckmate) { attackKingScore += boardManager_->getCurrentTurn() == WHITE ? -INFINITY : INFINITY; }

    score = materialScore_;
    score += pieceSquareScore_;
    score += attackKingScore;

    return score;
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
    float score = 0;
    const float materialScore_ = materialScore();
    const float pieceSquareScore_ = pieceSquareScore();
    const auto result = boardManager_->getGameResult();
    bool wasCheckmate = result & GameResult::CHECKMATE;

    if (boardManager_->getMoveHistory().size() > 0 && boardManager_->getMoveHistory().top().resultBits &
        MoveResult::CHECK_MATE) { wasCheckmate = true; }

    float attackKingScore = 0.f;
    // Invert the traditional scoring. If it's now white, it must have been black to move last

    if (wasCheckmate) { attackKingScore += boardManager_->getCurrentTurn() == WHITE ? -INFINITY : INFINITY; }

    score = materialScore_;
    score += pieceSquareScore_;
    score += attackKingScore;

    return score;
}
