//
// Created by jacks on 16/06/2025.
//

#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <string>
#include <cstdint>
#include <array>
#include <iostream>
#include <optional>

#include "Engine/Piece.h"

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

    constexpr std::array<uint64_t, 8> files = {
                buildFileBoard('a'),
                buildFileBoard('b'),
                buildFileBoard('c'),
                buildFileBoard('d'),
                buildFileBoard('e'),
                buildFileBoard('f'),
                buildFileBoard('g'),
                buildFileBoard('h')
            };

    constexpr std::array<uint64_t, 8> ranks = {
                buildRankBoard(1),
                buildRankBoard(2),
                buildRankBoard(3),
                buildRankBoard(4),
                buildRankBoard(5),
                buildRankBoard(6),
                buildRankBoard(7),
                buildRankBoard(8)
            };
}


class BitBoards {
public:

    BitBoards();

    void loadFEN(const std::string& fen);
    std::string &toFEN();

    [[nodiscard]] uint64_t getBitboard(const Piece& piece) const;
    uint64_t &operator[](const Piece piece){ return bitboards[piece]; }
    void printBitboard() const;

    std::optional<Piece> getPiece(const int rank, const int file) const;

private:

    // array of PIECE_N length bitboards
    std::array<uint64_t, Piece::PIECE_N> bitboards;
    std::string fen_{};
};


#endif //BITBOARDS_H
