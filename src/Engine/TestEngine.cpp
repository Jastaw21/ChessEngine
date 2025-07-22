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

float TestEngine::alphaBeta(const int depth, bool isMaximising, float alpha, float beta){
    if (depth == 0 || internalBoardManager_.isGameOver()) { return evaluate(); }

    auto moves = generateMoveList();
    std::ranges::sort(moves.begin(), moves.end(),
                      [&](const Move& a, const Move& b) { return a.resultBits & CAPTURE > b.resultBits & CAPTURE; });
    if (isMaximising) {
        float maxEval = -INFINITY;
        for (auto& move: moves) {
            internalBoardManager_.forceMove(move);
            float eval = alphaBeta(depth - 1, !isMaximising, alpha, beta);
            internalBoardManager_.undoMove();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) { break; }
        }
        return maxEval;
    }
    float minEval = INFINITY;
    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float eval = alphaBeta(depth - 1, !isMaximising, alpha, beta);
        internalBoardManager_.undoMove();
        minEval = std::min(minEval, eval);
        beta = std::min(beta, eval);
        if (beta <= alpha) { break; }
    }
    return minEval;
}


Move TestEngine::search(const int depth){
    auto moves = generateMoveList();
    if (moves.empty()) { return Move(); }

    Move bestMove = moves[0];
    float bestEval = -INFINITY;

    const Colours thisTurn = internalBoardManager_.getCurrentTurn();
    const bool isWhite = thisTurn == WHITE;

    for (auto& move: moves) {
        internalBoardManager_.forceMove(move);
        float eval = alphaBeta(depth - 1, !isWhite, -INFINITY, INFINITY);
        internalBoardManager_.undoMove();

        if (!isWhite) { eval = -eval; }

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
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

float TestEngine::evaluateMove(Move& move){
    const float scoreBefore = evaluate();
    internalBoardManager_.tryMove(move);
    const float scoreAfter = evaluate();
    internalBoardManager_.undoMove();

    const auto result = scoreAfter - scoreBefore;
    if (internalBoardManager_.getCurrentTurn() == WHITE) { return result; }

    return -result;
}

float TestEngine::evaluate(){
    float score = 0;
    const float materialScore_ = materialScore();
    const float pieceSquareScore_ = pieceSquareScore();

    float attackKingScore = 0.f;
    if (internalBoardManager_.getMoveHistory().top().resultBits & CHECK_MATE) {
        attackKingScore += internalBoardManager_.getCurrentTurn() == WHITE ? INFINITY : -INFINITY;
    }

    score = materialScore_ * Weights::MATERIAL_WEIGHT;
    score += pieceSquareScore_ * Weights::PIECE_SQUARE_SCORE;
    score += attackKingScore;
    return score;
}

