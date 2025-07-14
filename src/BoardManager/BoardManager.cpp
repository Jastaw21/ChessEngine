// ReSharper disable CppTooWideScopeInitStatement
#include "BoardManager/BoardManager.h"

#include <bitset>
#include <iostream>
#include <vector>

#include "BoardManager/BitBoards.h"
#include "BoardManager/Rules.h"

std::string Move::toUCI() const{
    return std::string{static_cast<char>('a' + fileFrom - 1)} + std::to_string(rankFrom) + static_cast<char>(
               'a' + fileTo - 1) + std::to_string(rankTo);
}


Move createMove(const Piece& piece, const std::string& moveUCI){
    const int fileFrom = moveUCI[0] - 'a' + 1;
    const int rankFrom = moveUCI[1] - '1' + 1;
    const int fileTo = moveUCI[2] - 'a' + 1;
    const int rankTo = moveUCI[3] - '1' + 1;

    return Move(piece, rankAndFileToSquare(rankFrom, fileFrom), rankAndFileToSquare(rankTo, fileTo));
}

BoardManager::BoardManager() = default;

bool BoardManager::prelimCheckMove(Move& move){
    move.resultBits = 0; // reset the move result tracker
    const int fromSquare = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const int toSquare = rankAndFileToSquare(move.rankTo, move.fileTo);

    const Bitboard toSquareBitboard = 1ULL << toSquare;
    const auto friendlyPieces = bitboards.getOccupancy(pieceColours[move.piece]);
    const auto enemyPieces = bitboards.getOccupancy() & ~friendlyPieces;

    const auto allMoves = magicBitBoards.getMoves(fromSquare, move.piece, bitboards);

    // move not any form of legal
    if (!(allMoves & toSquareBitboard)) {
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    // move would be on top of a friendly piece
    if (toSquareBitboard & friendlyPieces) {
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    // we are attacking an enemy, check it's legal (i.e. non-king)
    if (toSquareBitboard & enemyPieces) {
        if (handleCapture(move))
            return true;
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    // ep
    if (move.piece == WP || move.piece == BP) {
        const auto enPassantMoves = RulesCheck::getEnPassantVulnerableSquares(&bitboards, pieceColours[move.piece]);
        if (enPassantMoves & toSquareBitboard) {
            if (checkAndHandleEP(move))
                return true;
        }
        if (move.fileTo != move.fileFrom) {
            // if not EP or capture (handled above), going diagonal must be an illegal move
            move.resultBits = 0; // reset the move result tracker
            move.resultBits |= ILLEGAL_MOVE;
            return false;
        }
    }

    // castling
    const auto castlingMoves = RulesCheck::getCastlingMoves(fromSquare, move.piece, &bitboards);
    if (toSquareBitboard & castlingMoves) {
        move.resultBits |= CASTLING;
        return true;
    }

    // promotion

    // push
    move.resultBits |= PUSH;
    return true;
}

bool BoardManager::checkMove(Move& move){
    if (!prelimCheckMove(move)) { return false; }
    makeMove(move);
    if (friendlyKingInCheck(move)) {
        undoMove(move);
        move.resultBits |= KING_IN_CHECK;
        return false;
    }
    if (opponentKingInCheck(move)) {
        move.resultBits |= CHECK;
        if (isNowCheckMate()) { move.resultBits |= CHECK_MATE; }
    }
    undoMove(move);
    return true;
}

bool BoardManager::tryMove(Move& move){
    if (!checkMove(move)) { return false; }
    makeMove(move);
    return true;
}

bool BoardManager::forceMove(Move& move){
    makeMove(move);
    return true;
}

void BoardManager::makeMove(Move& move){
    // set the "from" square of the moving piece to zero

    bitboards.setZero(move.rankFrom, move.fileFrom);
    if (move.resultBits & CASTLING) {
        const auto relevantRook = move.piece == WK ? WR : BR; // what is the rook we also need to move?

        int movedRookFileTo = 0;
        int movedRookFileFrom = 0;
        // queen side
        if (move.fileTo == 3) {
            movedRookFileTo = move.fileTo + 1; // needs to move one inside the king
            movedRookFileFrom = 1; //... from file 1
        }
        // king side
        else if (move.fileTo == 7) {
            movedRookFileTo = move.fileTo - 1; // needs to move one inside the king
            movedRookFileFrom = 8; //... from file 8
        }

        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        bitboards.setOne(relevantRook, move.rankTo, movedRookFileTo);
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        bitboards.setZero(move.rankTo, movedRookFileFrom);
    }

    // check to see if discovered capture
    if (const auto discoveredPiece = bitboards.getPiece(move.rankTo, move.fileTo);
        discoveredPiece.has_value() && pieceColours[discoveredPiece.value()] != pieceColours[move.piece]) {
        move.resultBits |= CAPTURE;
        move.capturedPiece = discoveredPiece.value();
    }

    // if it was a normal capture, set that piece to zero
    if (move.resultBits & CAPTURE && !(move.resultBits & EN_PASSANT))
        bitboards.setZero(move.rankTo, move.fileTo);

    // if it was an en_passant capture, set the correct square to zero
    if (move.resultBits & EN_PASSANT) {
        const auto rankOffset = pieceColours[move.piece] == WHITE ? -1 : 1;
        bitboards.setZero(move.rankTo + rankOffset, move.fileTo);
    }

    // set the to square of the moving piece to one
    bitboards.setOne(move.piece, move.rankTo, move.fileTo);

    if (currentTurn == WHITE)
        currentTurn = BLACK;
    else
        currentTurn = WHITE;

    moveHistory.emplace(move);
}

void BoardManager::undoMove(const Move& move){
    // set the "from" bit back to one
    const auto squareFrom = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    bitboards[move.piece] |= 1ULL << squareFrom;

    // if it was a capture, restore that piece to one
    if (move.resultBits & CAPTURE && !(move.resultBits & EN_PASSANT)) {
        const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
        bitboards[move.capturedPiece] |= 1ULL << squareTo;
    }

    // if it was an en_passant capture, restore the correct square to one
    if (move.resultBits & EN_PASSANT) {
        const auto opponentPawn = pieceColours[move.piece] == WHITE ? BP : WP;
        const auto rankOffset = pieceColours[move.piece] == WHITE ? -1 : 1;
        bitboards.setOne(opponentPawn, move.rankTo + rankOffset, move.fileTo);
    }

    // if it was a castling, restore the rooks to their original positions
    if (move.resultBits & CASTLING) {
        bitboards.setZero(move.rankTo, move.fileTo);

        const auto relevantRook = move.piece == WK ? WR : BR;

        int movedRookFileTo;
        int movedRookFileFrom;
        // queen side
        if (move.fileTo == 3) {
            // set the new rook location
            movedRookFileTo = move.fileTo + 1;
            movedRookFileFrom = 1;
        }
        // king side
        else if (move.fileTo == 7) {
            movedRookFileTo = move.fileTo - 1;
            movedRookFileFrom = 8;
        }

        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        bitboards.setOne(relevantRook, move.rankFrom, movedRookFileFrom);
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        bitboards.setZero(move.rankTo, movedRookFileTo);
    }

    // set the "to" bit back to zero for this piece
    const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
    bitboards[move.piece] &= ~(1ULL << squareTo);

    moveHistory.pop();

    if (currentTurn == WHITE)
        currentTurn = BLACK;
    else
        currentTurn = WHITE;
}

void BoardManager::undoMove(){
    if (moveHistory.empty())
        return;
    undoMove(moveHistory.top());
}

bool BoardManager::opponentKingInCheck(Move& move){
    const auto& opponentKing = pieceColours[move.piece] == WHITE ? BK : WK;
    const auto& opponentKingLocation = bitboards[opponentKing];

    if (opponentKingLocation == 0) {
        return false; // No king on board
    }

    const auto attacks = RulesCheck::getAttackMoves(rankAndFileToSquare(move.rankTo, move.fileTo), move.piece,
                                                    &bitboards);

    if ((attacks & opponentKingLocation) != 0) {
        move.resultBits |= CHECK;
        return true;
    }

    return false;
}

bool BoardManager::isNowCheckMate(){ return !hasLegalMoveToEscapeCheck(); }

bool BoardManager::hasLegalMoveToEscapeCheck(){
    for (const auto& pieceName: filteredPieces[currentTurn]) { if (canPieceEscapeCheck(pieceName)) { return true; } }
    return false;
}

bool BoardManager::canPieceEscapeCheck(const Piece& pieceName){
    const auto pieceLocations = getStartingSquaresOfPiece(pieceName);

    for (const int startSquare: pieceLocations) {
        const auto possibleMoves = magicBitBoards.getMoves(startSquare, pieceName, bitboards);
        const auto destinationSquares = std::bitset<64>(possibleMoves);

        if (hasValidMoveFromSquare(pieceName, startSquare, destinationSquares)) { return true; }
    }
    return false;
}

bool BoardManager::hasValidMoveFromSquare(const Piece pieceName, const int startSquare,
                                          const std::bitset<64>& destinationSquares){
    for (int destinationSquare = 0; destinationSquare < destinationSquares.size(); ++destinationSquare) {
        if (!destinationSquares.test(destinationSquare)) { continue; }

        auto move = Move(pieceName, startSquare, destinationSquare);

        if (isValidEscapeMove(move)) { return true; }
    }
    return false;
}

bool BoardManager::isValidEscapeMove(Move& move){
    const bool moveSuccess = tryMove(move);
    const bool isValidEscape = !friendlyKingInCheck(move);
    if (moveSuccess)
        undoMove(move);
    return isValidEscape;
}

bool BoardManager::handleCapture(Move& move) const{
    const auto capturedPiece = bitboards.getPiece(move.rankTo, move.fileTo);

    if (!capturedPiece.has_value()) {
        std::cout << "Error: No piece captured" << std::endl;
        return false;
    }

    if (capturedPiece.value() == WK || capturedPiece.value() == BK) { return false; }

    move.capturedPiece = capturedPiece.value();
    move.resultBits |= CAPTURE;
    return true;
}

bool BoardManager::checkAndHandleEP(Move& move){
    if (moveHistory.empty()) { return false; }
    const auto lastMove = moveHistory.top();
    if (lastMove.piece != WP && lastMove.piece != BP) { return false; } // must be a pawn that's moved
    if (abs(lastMove.rankTo - lastMove.rankFrom) != 2) { return false; } // must be a pawn that has moved two squares
    if (move.fileTo != lastMove.fileFrom) { return false; }

    const int targetRankOffset = pieceColours[move.piece] == WHITE ? 1 : -1;
    // The offset is relative to the last pawn's to location. So white has to go north of the last move
    if (move.rankTo != lastMove.rankTo + targetRankOffset) { return false; }
    move.resultBits |= EN_PASSANT;
    move.resultBits |= CAPTURE;
    const auto relevantFile = move.fileTo;
    const auto rankOffset = pieceColours[move.piece] == WHITE ? -1 : 1;
    move.capturedPiece = bitboards.getPiece(move.rankTo + rankOffset, relevantFile).value();

    return true;
}


bool BoardManager::friendlyKingInCheck(const Move& move){
    const Colours friendlyColor = pieceColours[move.piece];
    const Piece friendlyKing = friendlyColor == BLACK ? BK : WK;
    const Bitboard kingLocation = bitboards[friendlyKing];

    if (kingLocation == 0) {
        return false; // No king on board
    }

    // Iterate through enemy pieces
    const auto colourToSearch = pieceColours[move.piece] == WHITE ? BLACK : WHITE;
    for (const auto& pieceName: filteredPieces[colourToSearch]) {
        const auto pieceLocations = getStartingSquaresOfPiece(pieceName);
        for (const int startSquare: pieceLocations) {
            // prelim check if anything could feasibly attack this square
            const Bitboard prelimAttacks = rules.getPseudoAttacks(pieceName, startSquare);
            if (!(kingLocation & prelimAttacks))
                continue;
            const Bitboard attackedSquares = RulesCheck::getAttackMoves(startSquare, pieceName, &bitboards);
            if ((attackedSquares & kingLocation) != 0) { return true; }
        }
    }

    return false;
}

std::vector<int> BoardManager::getStartingSquaresOfPiece(const Piece& piece){
    std::vector<int> startingSquares;
    startingSquares.reserve(8); // max of 8 pieces per board

    auto startingBoard = bitboards[piece];
    if (startingBoard == 0ULL) { return startingSquares; }
    while (startingBoard) {
        // count trailing zeros to find the index of the first set bit
        const int index = std::countr_zero(startingBoard);
        startingSquares.push_back(index);
        // clear the bit at that index
        startingBoard &= ~(1ULL << index);
    }

    return startingSquares;
}