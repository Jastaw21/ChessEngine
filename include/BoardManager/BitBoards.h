//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Engine/Piece.h"

typedef uint64_t Bitboard;

struct Move;

int rankAndFileToSquare(int rank, int file);
void squareToRankAndFile(int square, int& rank, int& file);
int squareToFile(int square);
int squareToRank(int square);

namespace Comparisons {
    constexpr Bitboard buildFileBoard(const char file){
        const int index = (file) - 'a';
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


class BitBoards {
public:

    BitBoards();

    void loadFEN(const std::string& fen);
    std::string &toFEN();

    [[nodiscard]] Bitboard getBitboard(const Piece& piece) const;
    Bitboard &operator[](const Piece piece){ return bitboards[piece]; }

    std::optional<Piece> getPiece(int rank, int file) const;
    std::optional<Piece> getPiece(int square) const;
    void setZero(int rank, int file);
    void setOne(const Piece& piece, int rank, int file);

    bool testBoard(Bitboard inBoard) const;
    bool testSquare(int square) const;

    int countPiece(const Piece& pieceToSearch) const;

    std::vector<Piece> getAttackingPieces(const Piece& piece);

    Bitboard getOccupancy() const;
    Bitboard getOccupancy(const Piece& piece) const;
    Bitboard getOccupancy(const Colours& colour) const;

private:

    // array of PIECE_N length bitboards
    std::array<Bitboard, PIECE_N> bitboards;
    std::string fen_{};
};


#endif //BITBOARDS_H