//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>

#include "Engine/Piece.h"
#include "Utility/Fen.h"
#include "Utility/Vec2D.h"

using Bitboard = uint64_t;

struct Move;


int rankAndFileToSquare(int rank, int file);

void squareToRankAndFile(int square, int& rank, int& file);

RankAndFile squareToRankAndFileStruct(const int square);

int squareToFile(int square);

int squareToRank(int square);

inline void printBitboard(const Bitboard inBoard){
    for (int rank = 8; rank >= 1; --rank) {
        std::cout << rank << " "; // print the rank label

        for (int file = 1; file <= 8; ++file) {
            auto targetSquare = rankAndFileToSquare(rank, file);
            if ((inBoard & (1ULL << targetSquare)) != 0) { std::cout << "x "; } else { std::cout << ". "; }
        }

        std::cout << std::endl;
    }

    std::cout << "r ";
    for (int file = 1; file <= 8; ++file) {
        const char fileText = 'a' + file - 1;
        std::cout << fileText << " ";
    }
    std::cout << std::endl;
}

int getLowestSetBit(const Bitboard& inBoard);
int popLowestSetBit(Bitboard& inBoard);

namespace Comparisons {
    constexpr Bitboard buildFileBoard(const char file){
        const int index = file - 'a';
        Bitboard result = 0ULL;
        for (int i = 0; i < 8; ++i) { result |= 1ULL << (index + 8 * i); }
        return result;
    }

    constexpr Bitboard buildRankBoard(const int rank){
        Bitboard result = 0ULL;
        const int base = (rank - 1) * 8;

        for (int i = 0; i < 8; ++i) { result |= 1ULL << (base + i); }
        return result;
    }

    constexpr std::array files = {
                buildFileBoard('a'),
                buildFileBoard('b'),
                buildFileBoard('c'),
                buildFileBoard('d'),
                buildFileBoard('e'),
                buildFileBoard('f'),
                buildFileBoard('g'),
                buildFileBoard('h')
            };

    constexpr std::array ranks = {
                buildRankBoard(1),
                buildRankBoard(2),
                buildRankBoard(3),
                buildRankBoard(4),
                buildRankBoard(5),
                buildRankBoard(6),
                buildRankBoard(7),
                buildRankBoard(8)
            };

    inline Bitboard north(const Bitboard& inBitBoard){ return inBitBoard << 8; }
    inline Bitboard south(const Bitboard& inBitBoard){ return inBitBoard >> 8; }
    inline Bitboard east(const Bitboard& inBitBoard){ return inBitBoard >> 1; }
    inline Bitboard west(const Bitboard& inBitBoard){ return inBitBoard << 1; }
    inline Bitboard northEast(const Bitboard& inBitBoard){ return inBitBoard >> 9; }
    inline Bitboard northWest(const Bitboard& inBitBoard){ return inBitBoard >> 7; }
    inline Bitboard southEast(const Bitboard& inBitBoard){ return inBitBoard << 7; }
    inline Bitboard southWest(const Bitboard& inBitBoard){ return inBitBoard << 9; }
}

namespace Constants {
    constexpr Bitboard RANK_1 = 0xff;
    constexpr Bitboard RANK_2 = 0xff00;
    constexpr Bitboard RANK_3 = 0xff0000;
    constexpr Bitboard RANK_4 = 0xff000000;
    constexpr Bitboard RANK_5 = 0xff00000000;
    constexpr Bitboard RANK_6 = 0xff0000000000;
    constexpr Bitboard RANK_7 = 0xff000000000000;
    constexpr Bitboard RANK_8 = 0xff00000000000000;

    constexpr Bitboard FILE_A = 0x101010101010101;
    constexpr Bitboard FILE_B = 0x202020202020202;
    constexpr Bitboard FILE_C = 0x404040404040404;
    constexpr Bitboard FILE_D = 0x808080808080808;
    constexpr Bitboard FILE_E = 0x1010101010101010;
    constexpr Bitboard FILE_F = 0x2020202020202020;
    constexpr Bitboard FILE_G = 0x4040404040404040;
    constexpr Bitboard FILE_H = 0x8080808080808080;

    constexpr Bitboard KING_SIDE_CASTLING = 0x6000000000000060;
    constexpr Bitboard QUEEN_SIDE_CASTLING = 0xe0000000000000e;

    constexpr Bitboard WHITE_KS_CASTLING = KING_SIDE_CASTLING & RANK_1;
    constexpr Bitboard WHITE_QS_CASTLING = QUEEN_SIDE_CASTLING & RANK_1;
    constexpr Bitboard BLACK_KS_CASTLING = KING_SIDE_CASTLING & RANK_8;
    constexpr Bitboard BLACK_QS_CASTLING = QUEEN_SIDE_CASTLING & RANK_8;

    constexpr Bitboard C1 = 0x4;
    constexpr Bitboard G1 = 0x40;

    constexpr Bitboard C8 = 0x400000000000000;
    constexpr Bitboard G8 = 0x4000000000000000;
}

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