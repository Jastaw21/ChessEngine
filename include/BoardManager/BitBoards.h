//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <array>
#include <optional>
#include <string>

#include "Engine/Piece.h"
#include "Utility/ChessUtility.h"
#include "Utility/Fen.h"


struct Move;


class BitBoards {
public:

    BitBoards();

    void setFenPositionOnly(const FenString& fen);
    std::string& toFEN();

    [[nodiscard]] Bitboard getBitboard(const Piece& piece) const;
    Bitboard& operator[](const Piece piece){ return bitboards[piece]; }

    std::optional<Piece> getPiece(int rank, int file) const;
    std::optional<Piece> getPiece(int square) const;
    void setZero(int rank, int file);
    void setOne(const Piece& piece, int rank, int file);

    bool testBoard(Bitboard inBoard) const;
    bool testSquare(int square) const;

    int countPiece(const Piece& pieceToSearch) const;

    Bitboard getOccupancy() const;
    Bitboard getOccupancy(const Piece& piece) const;
    Bitboard getOccupancy(const Colours& colour) const;

private:

    // array of PIECE_N length bitboards
    std::array<Bitboard, PIECE_N> bitboards;
    std::string fen_{};
};


#endif //BITBOARDS_H