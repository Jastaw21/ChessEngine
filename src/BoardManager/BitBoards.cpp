//
// Created by jacks on 16/06/2025.
//


#include "BoardManager/BitBoards.h"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "Engine/Piece.h"
#include <functional>

#include "BoardManager/Move.h"

BitBoards::BitBoards(){ bitboards.fill(0ULL); }

void BitBoards::setFenPositionOnly(const FenString& fen){
    fen_ = fen;
    bitboards.fill(0ULL);
    // starting from a8, h8 is 63

    int rank = 8;
    int file = 1;
    size_t i = 0;

    while (i < fen.size() && fen[i] != ' ') {
        if (char c = fen[i]; c == '/') {
            rank--;
            file = 1;
        } // Move to the next rank
        else if (isdigit(c))
            file += c - '0'; // Skip that many empty squares
        else {
            if (auto it = CHAR_TO_PIECE_MAP.find(c); it != CHAR_TO_PIECE_MAP.end()) {
                const int square = rankAndFileToSquare(rank, file);
                bitboards[it->second] |= 1ULL << square;
            }
            file++;
        }
        i++;
    }
}

Bitboard BitBoards::getBitboard(const Piece& piece) const{ return bitboards[piece]; }


std::optional<Piece> BitBoards::getPiece(const int rank, const int file) const{
    const int toSquare = rankAndFileToSquare(rank, file);

    // Check if the destination square is empty

    for (int pieceIndex = 0; pieceIndex < PIECE_N; ++pieceIndex) {
        // ReSharper disable once CppTooWideScopeInitStatement
        auto piece = static_cast<Piece>(pieceIndex);
        if ((bitboards[piece] & 1ULL << toSquare) != 0) { return {piece}; }
    }
    return {};
}

std::optional<Piece> BitBoards::getPiece(const int square) const{
    int rank, file;
    squareToRankAndFile(square, rank, file);
    return getPiece(rank, file);
}

void BitBoards::setZero(const int rank, const int file){
    const int toSquare = rankAndFileToSquare(rank, file);
    for (int pieceIndex = 0; pieceIndex < 12; pieceIndex++) {
        const auto piece = static_cast<Piece>(pieceIndex);
        bitboards[piece] &= ~(1ULL << toSquare);
    }
}

void BitBoards::setOne(const Piece& piece, const int rank, const int file){
    bitboards[piece] |= 1ULL << rankAndFileToSquare(rank, file);
}


bool BitBoards::testBoard(const Bitboard inBoard) const{
    const bool anyBoardMatches = std::ranges::any_of(bitboards, [&](const Bitboard& board) {
        return (board & inBoard) != 0;
    });
    return anyBoardMatches;
}

bool BitBoards::testSquare(const int square) const{
    const bool anyBoardMatches = std::ranges::any_of(bitboards, [&](const Bitboard& board) {
        return (board & (1ULL << square)) != 0;
    });
    return anyBoardMatches;
}

int BitBoards::countPiece(const Piece& pieceToSearch) const{ return std::popcount(bitboards[pieceToSearch]); }

Bitboard BitBoards::getOccupancy() const{
    return
            bitboards[WP] | bitboards[WN] | bitboards[WB] | bitboards[WR] | bitboards[WQ] | bitboards[WK]
            | bitboards[BP] | bitboards[BN] | bitboards[BB] | bitboards[BR] | bitboards[BQ] | bitboards[BK];
}

Bitboard BitBoards::getOccupancy(const Piece& piece) const{ return bitboards[piece]; }

Bitboard BitBoards::getOccupancy(const Colours& colour) const{
    if (colour == WHITE) {
        // no loop
        return bitboards[WP] | bitboards[WN] | bitboards[WB] | bitboards[WR] | bitboards[WQ] | bitboards[WK];
    }

    return bitboards[BP] | bitboards[BN] | bitboards[BB] | bitboards[BR] | bitboards[BQ] | bitboards[BK];
}


std::string& BitBoards::getFenPositionOnly(){
    fen_ = "";
    // rank by rank
    for (int rank = 8; rank >= 1; --rank) {
        // will be used to count empty squares
        int numEmpty = 0;

        // file by file
        for (int file = 1; file <= 8; ++file) {
            // square i.e. the bit we'll search
            const int square = rankAndFileToSquare(rank, file);

            bool isPieceHere = false;

            // check each piece
            for (const auto& [key, value]: CHAR_TO_PIECE_MAP) {
                // if the bit is in the bitboard, and-ed with the square, then we have a piece here
                if (bitboards[value] & 1ULL << square) {
                    if (numEmpty > 0) {
                        fen_ += std::to_string(numEmpty);
                        numEmpty = 0;
                    }
                    isPieceHere = true;
                    fen_ += key;
                    break;
                }
            }
            if (!isPieceHere)
                numEmpty++;
        }

        if (numEmpty > 0) {
            fen_ += std::to_string(numEmpty);
            numEmpty = 0;
        }
        if (rank != 1)
            fen_ += '/';
    }

    return fen_;
}

void BitBoards::applyMove(Move& moveToApply){
    setZero(moveToApply.rankFrom, moveToApply.fileFrom);

    // castling needs special stuff doing
    if (moveToApply.resultBits & CASTLING) { applyCastlingMove(moveToApply); }

    // if it was a normal capture, set the piece to zero
    if (moveToApply.resultBits & CAPTURE && !(moveToApply.resultBits & EN_PASSANT))
        setZero(moveToApply.rankTo, moveToApply.fileTo);

    // if it was an en_passant capture, set the correct square to zero
    if (moveToApply.resultBits & EN_PASSANT) {
        const auto rankOffset = moveToApply.piece == WP ? -1 : 1;
        setZero(moveToApply.rankTo + rankOffset, moveToApply.fileTo);
    }

    // if it's not a promotion, set the to square of the moving piece to one
    if (!(moveToApply.resultBits & PROMOTION)) { setOne(moveToApply.piece, moveToApply.rankTo, moveToApply.fileTo); }

    // otherwise, need to toggle on the bit for the piece it chose
    else { setOne(moveToApply.promotedPiece, moveToApply.rankTo, moveToApply.fileTo); }
}

void BitBoards::undoMove(const Move& moveToUndo){
    const auto squareFrom = rankAndFileToSquare(moveToUndo.rankFrom, moveToUndo.fileFrom);
    bitboards[moveToUndo.piece] |= 1ULL << squareFrom;

    // if it was a capture, restore that piece to one
    if (moveToUndo.resultBits & CAPTURE && !(moveToUndo.resultBits & EN_PASSANT)) {
        const auto squareTo = rankAndFileToSquare(moveToUndo.rankTo, moveToUndo.fileTo);
        bitboards[moveToUndo.capturedPiece] |= 1ULL << squareTo;
    }

    // if it was an en_passant capture, restore the correct square to one
    if (moveToUndo.resultBits & EN_PASSANT) {
        const auto opponentPawn = moveToUndo.piece == WP ? BP : WP;
        const auto rankOffset = moveToUndo.piece == WP ? -1 : 1;
        setOne(opponentPawn, moveToUndo.rankTo + rankOffset, moveToUndo.fileTo);
    }

    // if it was a castling moveToUndo, restore the rooks to their original positions
    if (moveToUndo.resultBits & CASTLING) { undoCastlingMove(moveToUndo); }

    // set the "to" bit back to zero for this piece
    const auto squareTo = rankAndFileToSquare(moveToUndo.rankTo, moveToUndo.fileTo);
    if (moveToUndo.resultBits & PROMOTION) {
        bitboards[moveToUndo.promotedPiece] &= ~(1ULL << squareTo); // whatever we promoted to is gone
    }

    bitboards[moveToUndo.piece] &= ~(1ULL << squareTo);
}

void BitBoards::applyCastlingMove(const Move& moveToApply){
    const auto relevantRook = moveToApply.piece == WK ? WR : BR; // what is the rook we also need to moveToApply?

    int movedRookFileTo = 0;
    int movedRookFileFrom = 0;
    // queen side
    if (moveToApply.fileTo == 3) {
        movedRookFileTo = moveToApply.fileTo + 1; // needs to moveToApply one inside the king
        movedRookFileFrom = 1; //... from file 1
    }
    // king side
    else if (moveToApply.fileTo == 7) {
        movedRookFileTo = moveToApply.fileTo - 1; // needs to moveToApply one inside the king
        movedRookFileFrom = 8; //... from file 8
    }

    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    setOne(relevantRook, moveToApply.rankTo, movedRookFileTo);
    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    setZero(moveToApply.rankTo, movedRookFileFrom);
}

void BitBoards::undoCastlingMove(const Move& moveToUndo){
    setZero(moveToUndo.rankTo, moveToUndo.fileTo);

    const auto relevantRook = moveToUndo.piece == WK ? WR : BR;

    int movedRookFileTo;
    int movedRookFileFrom;
    // queen side
    if (moveToUndo.fileTo == 3) {
        // set the new rook location
        movedRookFileTo = moveToUndo.fileTo + 1;
        movedRookFileFrom = 1;
    }
    // king side
    else if (moveToUndo.fileTo == 7) {
        movedRookFileTo = moveToUndo.fileTo - 1;
        movedRookFileFrom = 8;
    }

    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    setOne(relevantRook, moveToUndo.rankFrom, movedRookFileFrom);
    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    setZero(moveToUndo.rankTo, movedRookFileTo);
}