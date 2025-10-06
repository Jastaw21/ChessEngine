//
// Created by jacks on 06/10/2025.
//

#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H


#include "Engine/Piece.h"

struct RankAndFile;

enum MoveResult {
    // successes
    PUSH = 1 << 0, // 0000 0001 - 1
    CAPTURE = 1 << 1, // 0000 0010 - 2
    EN_PASSANT = 1 << 2, // 0000 0100 - 4
    CASTLING = 1 << 3, // 0000 1000 - 8
    CHECK = 1 << 4, // 0001 0000 - 16

    // failures
    ILLEGAL_MOVE = 1 << 5, // 0010 0000 - 32
    PROMOTION = 1 << 6, // 0100 0000 - 64
    CHECK_MATE = 1 << 7, // 1000 0000 - 128
};

struct Move {
    Move() = default;

    Move(const Piece& piece, const int fromSquare, const int toSquare);

    Move(const Piece& piece, const int rankFrom, const int fileFrom, const int rankTo, const int fileTo) : piece(piece),
        rankFrom(rankFrom), fileFrom(fileFrom), rankTo(rankTo), fileTo(fileTo){};
    Piece piece = PIECE_N;
    int rankFrom = 0;
    int fileFrom = 0;
    int rankTo = 0;
    int fileTo = 0;

    int resultBits = 0ULL;
    Piece capturedPiece = PIECE_N;
    Piece promotedPiece = PIECE_N;

    std::string toUCI() const;

    bool operator==(const Move& other) const{
        return
                piece == other.piece
                && rankFrom == other.rankFrom
                && fileFrom == other.fileFrom
                && rankTo == other.rankTo
                && fileTo == other.fileTo
                && capturedPiece == other.capturedPiece
                && promotedPiece == other.promotedPiece;
    }

    bool operator!=(const Move& other) const{ return !(*this == other); }

    bool isInverseOf(const Move& move) const{
        return piece == move.piece
               && rankFrom == move.rankTo
               && fileFrom == move.fileTo
               && rankTo == move.rankFrom
               && fileTo == move.fileFrom;
    }
};

Move createMove(const Piece& piece, const std::string& moveUCI);
Move createMove(const Piece& piece, const RankAndFile squareFrom, const RankAndFile squareTo,
                const Piece promotedPiece = PIECE_N);
#endif //CHESS_MOVE_H