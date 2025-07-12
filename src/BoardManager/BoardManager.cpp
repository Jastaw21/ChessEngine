///
// Created by jacks on 18/06/2025.
//

// ReSharper disable CppTooWideScopeInitStatement
#include "BoardManager/BoardManager.h"

#include <bitset>
#include <iostream>
#include <vector>

#include "BoardManager/BitBoards.h"
#include "BoardManager/Rules.h"

std::string Move::toUCI() const{
    std::string uci;
    const char fileFromChar = 'a' + fileFrom - 1;
    uci += fileFromChar;
    uci += std::to_string(rankFrom);
    const char fileToChar = 'a' + fileTo - 1;
    uci += fileToChar;
    uci += std::to_string(rankTo);

    return uci;
}

Move createMove(const Piece& piece, const std::string& moveUCI){
    const char fromFile = moveUCI[0];
    const int fileFrom = fromFile - 'a' + 1;

    const char fromRank = moveUCI[1];
    const int rankFrom = fromRank - '1' + 1;

    const char toFile = moveUCI[2];
    const int fileTo = toFile - 'a' + 1;

    const char toRank = moveUCI[3];
    const int rankTo = toRank - '1' + 1;

    const Move newMove = {
                .piece = piece, .rankFrom = rankFrom, .fileFrom = fileFrom, .rankTo = rankTo, .fileTo = fileTo
            };

    return newMove;
}


BoardManager::BoardManager() = default;

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

bool BoardManager::checkMove(Move& move){
    if (!prelimCheckMove(move)) { return false; }
    makeMove(move);
    if (friendlyKingInCheck(move)) {
        undoMove(move);
        move.resultBits |= KING_IN_CHECK;
        return false;
    }
    if (opponentKingInCheck(move)) { move.resultBits |= CHECK; }
    undoMove(move);
    return true;
}

Bitboard BoardManager::getSliderMoves(const Piece& movePiece, const int fromSquare, const Bitboard& board){
    switch (movePiece) {
        case WR:
        case BR:
            return magicBitBoards.getRookAttacks(fromSquare, board);

        case WQ:
        case BQ:
            return magicBitBoards.getBishopAttacks(fromSquare, board) | magicBitBoards.
                   getRookAttacks(fromSquare, board);

        case WB:
        case BB:
            return magicBitBoards.getBishopAttacks(fromSquare, board);

        // ReSharper disable once CppDFAUnreachableCode
        default:
            return 0ULL;
    }
}

bool BoardManager::handleCapture(Move& move){
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

void BoardManager::handleEP(Move& move){
    const auto relevantFile = move.fileTo;
    const auto rankOffset = pieceColours[move.piece] == WHITE ? -1 : 1;
    move.capturedPiece = bitboards.getPiece(move.rankTo + rankOffset, relevantFile).value();
}

bool BoardManager::prelimCheckMove(Move& move){
    move.resultBits = 0; // reset the move result tracker
    const int fromSquare = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const int toSquare = rankAndFileToSquare(move.rankTo, move.fileTo);

    Bitboard toSquareBitboard = 1ULL << toSquare;
    const auto allPieces = bitboards.getOccupancy();
    const auto friendlyPieces = bitboards.getOccupancy(pieceColours[move.piece]);
    const auto enemyPieces = bitboards.getOccupancy() & ~friendlyPieces;

    Bitboard allMoves = 0ULL;
    // sliders, use magic BBs
    if (move.piece == WQ || move.piece == BQ ||
        move.piece == WR || move.piece == BR ||
        move.piece == WB || move.piece == BB) {
        allMoves = getSliderMoves(move.piece, fromSquare, bitboards.getOccupancy());
    }
    // all other pieces use the classic method   
    else { allMoves = RulesCheck::getPseudoLegalMoves(fromSquare, move.piece, &bitboards); }

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
            move.resultBits |= EN_PASSANT;
            move.resultBits |= CAPTURE;
            handleEP(move);
            return true;
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

bool BoardManager::tryMove(Move& move){
    if (!checkMove(move)) { return false; }
    makeMove(move);
    return true;
}

void BoardManager::makeMove(Move& move){
    // set the "from" square of the moving piece to zero
    bitboards.setZero(move.rankFrom, move.fileFrom);
    if (move.resultBits & CASTLING) {
        const auto relevantRook = move.piece == WK ? WR : BR; // what is the rook we also need to move?

        int movedRookFileTo;
        int movedRookFileFrom;
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

    moveHistory.push(move);
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

bool BoardManager::moveIsEnPassant(Move& move){
    if (abs(moveHistory.top().rankTo - moveHistory.top().rankFrom) != 2)
        return false;
    const Bitboard attackedSquare = 1ULL << rankAndFileToSquare(move.rankTo, move.fileTo);
    const auto& otherPawnPiece = pieceColours[move.piece] == WHITE ? BP : WP;
    const auto& locationsOfOtherPawns = bitboards[otherPawnPiece];
    const std::bitset<64> locationsOfOtherPawnsBits(locationsOfOtherPawns);
    int offset = pieceColours[move.piece] == WHITE ? 8 : -8;

    // track the locations of the opponent's pawns, shifted correctly
    Bitboard otherPawnLocations = 0ULL;
    for (int bit = 0; bit < locationsOfOtherPawnsBits.size(); ++bit) {
        if (locationsOfOtherPawnsBits.test(bit) && bit >= 8 && bit <= 55) {
            otherPawnLocations |= 1ULL << (bit + offset);
        }
    }

    const bool isEnPassant = (attackedSquare & otherPawnLocations) > 0 && abs(
                                 moveHistory.top().rankTo - moveHistory.top().rankFrom) == 2;

    if (!isEnPassant)
        return false;

    move.resultBits |= EN_PASSANT;
    move.capturedPiece = otherPawnPiece;
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
    for (int pieceIndex = 0; pieceIndex < PIECE_N; pieceIndex++) {
        const auto pieceName = static_cast<Piece>(pieceIndex);
        if (pieceColours[move.piece] == pieceColours[pieceName])
            continue;

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

    const auto& pieceLocation = std::bitset<64>(bitboards[piece]);
    if (!pieceLocation.any())
        return startingSquares;

    for (int index = 0; index < pieceLocation.size(); ++index) {
        if (pieceLocation.test(index)) { startingSquares.push_back(index); }
    }
    return startingSquares;
}











