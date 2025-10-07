//
// Created by jacks on 16/06/2025.
//

#include "Engine/MainEngine.h"

constexpr int WHITE_PROMOTION_START = 56;
constexpr int BLACK_PROMOTION_END = 7;
constexpr std::array<char, 4> PROMOTED_PIECES_WHITE = {'Q', 'N', 'B', 'R'};
constexpr std::array<char, 4> PROMOTED_PIECES_BLACK = {'q', 'n', 'b', 'r'};


void MainEngine::processPawnPromotion(std::vector<Move>& validMoves, Move& baseMove){
    const auto& piecesToCheck = (baseMove.piece == WP) ? PROMOTED_PIECES_WHITE : PROMOTED_PIECES_BLACK;

    for (const auto& pieceChar: piecesToCheck) {
        auto promotionMove = createMove(baseMove.piece, baseMove.toUCI() + pieceChar);
        // need to check the move doesnt leave you in check
        if (boardManager()->checkMove(promotionMove)) { validMoves.push_back(promotionMove); }
    }
}


void MainEngine::generateValidMovesFromPosition(const Piece& piece,
                                                const int startSquare, std::vector<Move>& moveList){
    auto possibleMoves = internalBoardManager_.getMagicBitBoards()->getMoves(
        startSquare,
        piece,
        *internalBoardManager_.getBitboards()
    );

    while (possibleMoves) {
        const auto endSquare = std::countr_zero(possibleMoves); // bottom set bit
        possibleMoves &= possibleMoves - 1; // pop the bit
        auto candidateMove = Move(piece, startSquare, endSquare);

        if ((piece == WP && endSquare >= WHITE_PROMOTION_START) ||
            (piece == BP && endSquare <= BLACK_PROMOTION_END)) {
            processPawnPromotion(moveList, candidateMove);
            continue;
        }
        // still need to check the move in case of checks etc
        if (internalBoardManager_.checkMove(candidateMove)) { moveList.push_back(candidateMove); }
    }
}

void MainEngine::generateMovesForPiece(const Piece& piece, std::vector<Move>& moveList){
    auto piecePositions = internalBoardManager_.getBitboards()->getBitboard(piece);

    while (piecePositions) {
        const auto startSquare = std::countr_zero(piecePositions); // bottom set bit
        piecePositions &= ~(1ULL << startSquare); // pop the bit
        generateValidMovesFromPosition(piece, startSquare, moveList);
    }
}

std::vector<Move> MainEngine::generateMoveList(){
    std::vector<Move> moves;
    moves.reserve(40);
    // check each piece we have
    const auto ourPieces = internalBoardManager_.getCurrentTurn() == WHITE
                               ? std::array{WP, WN, WB, WR, WQ, WK}
                               : std::array{BP, BN, BB, BR, BQ, BK};
    for (const auto& pieceName: ourPieces) { generateMovesForPiece(pieceName, moves); }
    return moves;
}

void MainEngine::setFullFen(const std::string& fen){ internalBoardManager_.setFullFen(fen); }

bool MainEngine::sendCommand(const std::string& command){
    parseUCI(command);
    return true;
}

std::string MainEngine::readResponse(){ return ""; }