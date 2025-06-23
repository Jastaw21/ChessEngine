//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <array>
#include <cstdint>
#include <optional>
#include <string>

#include "Engine/Piece.h"

struct Move;

/**
 * Converts a given chessboard rank and file to a corresponding square index.
 *
 * The rank and file are 1-based, where rank 1 is the bottom-most row,
 * rank 8 is the top-most row, file 1 is the left-most column (from white's perspective),
 * and file 8 is the right-most column.
 *
 * Square indices are 0-based and calculated sequentially from a8 (top-left, index 0)
 * to h1 (bottom-right, index 63) when traversing rows in descending order and columns
 * in ascending order.
 *
 * @param rank The rank of the square, must be in the range [1, 8].
 * @param file The file of the square, must be in the range [1, 8].
 * @return The corresponding square index in the range [0, 63].
 * @throws std::invalid_argument if rank or file is outside the range [1, 8].
 */
int rankAndFileToSquare(int rank, int file);
void squareToRankAndFile(int square, int& rank, int& file);

namespace Comparisons {
    constexpr uint64_t buildFileBoard(const char file){
        const int index = (file) - 'a';
        uint64_t result = 0ULL;
        for (int i = 0; i < 8; ++i) { result |= 1ULL << (index + 8 * i); }
        return result;
    }

    constexpr uint64_t buildRankBoard(const int rank){
        uint64_t result = 0ULL;
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

    inline uint64_t north(const uint64_t& inBitBoard){ return inBitBoard << 8; }
    inline uint64_t south(const uint64_t& inBitBoard){ return inBitBoard >> 8; }
    inline uint64_t east(const uint64_t& inBitBoard){ return inBitBoard >> 1; }
    inline uint64_t west(const uint64_t& inBitBoard){ return inBitBoard << 1; }
    inline uint64_t northEast(const uint64_t& inBitBoard){ return inBitBoard >> 9; }
    inline uint64_t northWest(const uint64_t& inBitBoard){ return inBitBoard >> 7; }
    inline uint64_t southEast(const uint64_t& inBitBoard){ return inBitBoard << 7; }
    inline uint64_t southWest(const uint64_t& inBitBoard){ return inBitBoard << 9; }
}


class BitBoards {
public:

    BitBoards();

    void loadFEN(const std::string& fen);
    std::string &toFEN();

    [[nodiscard]] uint64_t getBitboard(const Piece& piece) const;
    uint64_t &operator[](const Piece piece){ return bitboards[piece]; }

    std::optional<Piece> getPiece(int rank, int file) const;
    std::optional<Piece> getPiece(int square) const;
    void setZero(int rank, int file);
    void setOne(const Piece& piece, int rank, int file);

    bool test(uint64_t inBoard) const;

private:

    // array of PIECE_N length bitboards
    std::array<uint64_t, PIECE_N> bitboards;
    std::string fen_{};
};


#endif //BITBOARDS_H