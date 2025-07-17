//
// Created by jacks on 16/06/2025.
//

#include "Engine/TestEngine.h"


#include "BoardManager/Rules.h"
#include "EngineShared/PerftResults.h"

std::unordered_map<Piece, int> pieceValues = {
            {WP, 1},
            {BP, 1},

            {WR, 2},
            {BR, 2},

            {WN, 3},
            {BN, 3},

            {WB, 3},
            {BB, 3},

            {WK, 99},
            {BK, 99},

            {WQ, 20},
            {BQ, 20}
        };


TestEngine::TestEngine(const Colours colour) : EngineBase(colour){}

TestEngine::TestEngine(const Colours colour, BoardManager* boardManager): EngineBase(colour){
    setManager(boardManager);
}


float TestEngine::evaluate(BoardManager& mgr) const{
    float materialScore = 0.0f;

    for (int piece = 0; piece < PIECE_N; ++piece) {
        auto pieceName = static_cast<Piece>(piece);
        const int pieceCount = mgr.getBitboards()->countPiece(pieceName);
        const int pieceValue = pieceValues[pieceName];

        if (pieceColours[pieceName] == colour) { materialScore += pieceValue * pieceCount; } else {
            materialScore -= pieceValue * pieceCount;
        }
    }
    return materialScore;
}

float TestEngine::minMax(BoardManager& mgr, const int depth, const bool isMaximising){
    if (depth == 0) { return evaluate(mgr); }

    auto moves = generateMoveList(mgr);

    if (moves.empty()) { return evaluate(mgr); }

    int bestEval = isMaximising ? -100000 : 100000;

    for (Move& move: moves) {
        mgr.forceMove(move);
        int eval = minMax(mgr, depth - 1, !isMaximising);
        mgr.undoMove();

        if (isMaximising) { bestEval = std::max(bestEval, eval); } else { bestEval = std::min(bestEval, eval); }
    }
    return bestEval;
}


Move TestEngine::search(const int depth){
    BoardManager startingManager = *boardManager_;

    auto moves = generateMoveList(startingManager);
    if (moves.empty()) { return Move(); }

    Move bestMove = moves[0];
    float bestEval = -100000;

    const Colours thisTurn = startingManager.getCurrentTurn();

    for (auto& move: moves) {
        startingManager.forceMove(move);
        const float eval = minMax(startingManager, 2, thisTurn == colour);
        startingManager.undoMove();

        if (thisTurn == colour) {
            if (eval > bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        } else {
            if (eval < bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        }
    }
    return bestMove;
}

std::vector<Move> TestEngine::generateValidMovesFromPosition(BoardManager& mgr, const Piece& piece,
                                                             const int startSquare){
    std::vector<Move> validMoves;

    auto possibleMoves2 = mgr.getMagicBitBoards()->getMoves(startSquare, piece, *mgr.getBitboards());

    while (possibleMoves2) {
        const auto endSquare = std::countr_zero(possibleMoves2); // bottom set bit
        possibleMoves2 &= ~(1ULL << endSquare); // pop the bit
        auto candidateMove = Move(piece, startSquare, endSquare);
        if (mgr.checkMove(candidateMove))
            validMoves.push_back(candidateMove);
    }
    return validMoves;
}


std::vector<Move> TestEngine::generateMovesForPiece(BoardManager& mgr, const Piece& piece){
    std::vector<Move> pieceMoves;

    auto piecePositions = mgr.getBitboards()->getBitboard(piece);

    while (piecePositions) {
        const auto startSquare = std::countr_zero(piecePositions); // bottom set bit
        piecePositions &= ~(1ULL << startSquare); // pop the bit
        auto validMoves = generateValidMovesFromPosition(mgr, piece, startSquare);
        pieceMoves.insert(pieceMoves.end(), validMoves.begin(), validMoves.end());
    }

    return pieceMoves;
}

std::vector<Move> TestEngine::generateMoveList(BoardManager& mgr){
    std::vector<Move> moves;
    // check each piece we have
    for (const auto& pieceName: filteredPieces[mgr.getCurrentTurn()]) {
        auto pieceMoves = generateMovesForPiece(mgr, pieceName);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }
    return moves;
}

int TestEngine::simplePerft(const int depth, BoardManager& boardManager){
    if (depth == 0)
        return 1;

    int nodes = 0;
    auto moves = generateMoveList(boardManager);
    for (Move& move: moves) {
        boardManager.forceMove(move);
        nodes += simplePerft(depth - 1, boardManager);
        boardManager.undoMove();
    }

    return nodes;
}

PerftResults TestEngine::perft(const int depth, BoardManager& boardManager){
    if (depth == 0) return PerftResults{1, 0, 0, 0, 0, 0};

    PerftResults result{0, 0, 0, 0, 0, 0};
    auto moves = generateMoveList(boardManager);

    for (auto& move: moves) {
        boardManager.forceMove(move); // moves should be checked for legality already at this point so don't even worry
        PerftResults child = perft(depth - 1, boardManager);

        if (depth == 1) {
            result.nodes++;
            // Only increment move result types at leaf depth
            if (move.resultBits & CHECK) { result.checks++; }
            if (move.resultBits & EN_PASSANT) { result.enPassant++; }
            if (move.resultBits & CAPTURE) { result.captures++; }
            if (move.resultBits & CASTLING) { result.castling++; }
            if (move.resultBits & CHECK_MATE) { result.checkMate++; }
        } else { result += child; }

        boardManager.undoMove();
    }
    return result;
}


std::vector<PerftResults> TestEngine::perftDivide(const int depth, BoardManager& boardManager){
    auto moves = generateMoveList(boardManager);
    std::vector<PerftResults> results;

    for (auto& move: moves) {
        auto result = PerftResults();
        result.nodes = 0;
        result.fen = move.toUCI();
        boardManager.forceMove(move);
        result += perft(depth - 1, boardManager);
        boardManager.undoMove();

        results.push_back(result);
    }

    return results;
}

float TestEngine::evaluate(BoardManager& mgr){ return 0.0f; }

