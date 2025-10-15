//
// Created by jacks on 15/10/2025.
//

#include "Engine/MoveGenerator.h"
constexpr int WHITE_PROMOTION_START = 56;
constexpr int BLACK_PROMOTION_END = 7;
constexpr std::array<char, 4> PROMOTED_PIECES_BLACK = {'q', 'n', 'b', 'r'};
constexpr std::array<char, 4> PROMOTED_PIECES_WHITE = {'Q', 'N', 'B', 'R'};

Moves MoveGenerator::getMoves(BoardManager& manager){
    std::vector<Move> moves;
    moves.reserve(40);
    // check each piece we have
    const auto ourPieces = manager.getCurrentTurn() == WHITE
                               ? std::array{WP, WN, WB, WR, WQ, WK}
                               : std::array{BP, BN, BB, BR, BQ, BK};
    for (const auto& pieceName: ourPieces) { generateMovesForPiece(pieceName, moves, manager); }
    return moves;
}

void MoveGenerator::processPawnPromotion(std::vector<Move>& moves, Move& move, BoardManager& manager){
    const auto& piecesToCheck = (move.piece == WP) ? PROMOTED_PIECES_WHITE : PROMOTED_PIECES_BLACK;

    for (const auto& pieceChar: piecesToCheck) {
        auto promotionMove = createMove(move.piece, move.toUCI() + pieceChar);
        // need to check the move doesn't leave you in check
        if (manager.checkMove(promotionMove)) { moves.push_back(promotionMove); }
    }
}

void MoveGenerator::generateValidMovesFromPosition(const Piece piece, const int startSquare, std::vector<Move>& moves,
                                                   BoardManager& manager){
    auto possibleMoves = manager.getMagicBitBoards()->getMoves(
        startSquare,
        piece,
        *manager.getBitboards()
    );

    while (possibleMoves) {
        const auto endSquare = std::countr_zero(possibleMoves); // bottom set bit
        possibleMoves &= possibleMoves - 1; // pop the bit
        auto candidateMove = Move(piece, startSquare, endSquare);

        if ((piece == WP && endSquare >= WHITE_PROMOTION_START) ||
            (piece == BP && endSquare <= BLACK_PROMOTION_END)) {
            processPawnPromotion(moves, candidateMove, manager);
            continue;
        }
        // still need to check the move in case of checks etc
        if (manager.checkMove(candidateMove)) { moves.push_back(candidateMove); }
    }
}

void MoveGenerator::generateMovesForPiece(const Piece& piece, std::vector<Move>& moveList, BoardManager& manager){
    auto piecePositions = manager.getBitboards()->getBitboard(piece);

    while (piecePositions) {
        const auto startSquare = std::countr_zero(piecePositions); // bottom set bit
        piecePositions &= ~(1ULL << startSquare); // pop the bit
        generateValidMovesFromPosition(piece, startSquare, moveList, manager);
    }
}