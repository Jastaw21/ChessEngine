//
// Created by jacks on 25/06/2025.
//

#include <cstdint>

#include "BitBoards.h"

namespace SingleMoves {
    inline void southInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL >> 8; }
    inline void northInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL << 8; }
    inline void eastInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL >> 1; }
    inline void westInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL << 1; }
    inline void northEastInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL >> 9; }
    inline void northWestInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL >> 7; }
    inline void southEastInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL << 7; }
    inline void southWestInPlace(uint64_t& inBitBoard){ inBitBoard &= 1ULL << 9; }

    inline int square_south(const int square){ return square - 8; }
    inline int square_north(const int square){ return square + 8; }
    inline int square_east(const int square){ return square + 1; }
    inline int square_west(const int square){ return square - 1; }
    inline int square_northEast(const int square){ return square + 9; }
    inline int square_northWest(const int square){ return square + 7; }
    inline int square_southEast(const int square){ return square - 7; }
    inline int square_southWest(const int square){ return square - 9; }


    inline uint64_t south(const int square){
        if (squareToRank(square) == 1)
            return 0ULL;
        return 1ULL << square_south(square);
    }

    inline uint64_t north(const int square){
        if (squareToRank(square) == 8)
            return 0ULL;
        return 1ULL << square_north(square);
    }

    inline uint64_t east(const int square){
        if (squareToFile(square) == 8)
            return 0ULL;
        return 1ULL << square_east(square);
    }

    inline uint64_t west(const int square){
        if (squareToFile(square) == 1)
            return 0ULL;
        return 1ULL << square_west(square);
    }

    inline uint64_t northEast(const int square){
        if (squareToFile(square) == 8 || squareToRank(square) == 8)
            return 0ULL;
        return 1ULL << square_northEast(square);
    }

    inline uint64_t northWest(const int square){
        if (squareToFile(square) == 1 || squareToRank(square) == 8)
            return 0ULL;
        return 1ULL << square_northWest(square);
    }

    inline uint64_t southEast(const int square){
        if (squareToFile(square) == 8 || squareToRank(square) == 1)
            return 0ULL;
        return 1ULL << square_southEast(square);
    }

    inline uint64_t southWest(const int square){
        if (squareToFile(square) == 1 || squareToRank(square) == 1)
            return 0ULL;
        return 1ULL << square_southWest(square);
    }

    inline uint64_t allDirs(const int square){
        return north(square) | south(square) | east(square) | west(square) | northEast(square) | northWest(square) |
               southEast(square) | southWest(square);
    }


    inline uint64_t getKnightMoves(const int square){
        //{{2,-1}, {2,1}, {1,2},{-1,2},{-2,1},{-2,-1},{-1 -2}}

        uint64_t result = 0ULL;

        // plus 2 block
        {
            if (squareToRank(square) < 7) { const auto files = {-1, 1};
                for (const auto file: files) {
                    if (squareToFile(square) + file >= 1 && squareToFile(square) + file <= 8) {
                        result |= 1ULL << rankAndFileToSquare(squareToRank(square) + 2, squareToFile(square) + file);
                    }
                }
            }
        }

        // plus 1 block
        {
            if (squareToRank(square) < 8) { const auto files = {-2, 2};
                for (const auto file: files) {
                    if (squareToFile(square) + file >= 1 && squareToFile(square) + file <= 8) {
                        result |= 1ULL << rankAndFileToSquare(squareToRank(square) + 1, squareToFile(square) + file);
                    }
                }
            }
        }

        // minus 1 block
        {
            if (squareToRank(square) > 1) { const auto files = {-2, 2};
                for (const auto file: files) {
                    if (squareToFile(square) + file >= 1 && squareToFile(square) + file <= 8) {
                        result |= 1ULL << rankAndFileToSquare(squareToRank(square) -1, squareToFile(square) + file);
                    }
                }
            }
        }

        // minus 2 block
        {
            if (squareToRank(square) > 2) { const auto files = {-1, 1};
                for (const auto file: files) {
                    if (squareToFile(square) + file >= 1 && squareToFile(square) + file <= 8) {
                        result |= 1ULL << rankAndFileToSquare(squareToRank(square) - 2, squareToFile(square) + file);
                    }
                }
            }
        }

        return result;

        // do the plus 2 first
    }
}

namespace CrossBoardMoves {
    inline uint64_t wholeFile(const int startSquare){
        uint64_t result = 0ULL;
        const int file = squareToFile(startSquare);
        for (int rank = 1; rank <= 8; rank++) {
            if (rank == squareToRank(startSquare))
                continue;
            result |= 1ULL << rankAndFileToSquare(rank, file);
        }
        return result;
    }

    inline uint64_t wholeRank(const int startSquare){
        uint64_t result = 0ULL;
        const int rank = squareToRank(startSquare);
        for (int file = 1; file <= 8; file++) {
            if (file == squareToFile(startSquare))
                continue;
            result |= 1ULL << rankAndFileToSquare(rank, file);
        }
        return result;
    }

    inline uint64_t diags(const int startSquare){
        uint64_t result = 0ULL;
        int rank, file;
        squareToRankAndFile(startSquare, rank, file);

        // go north west
        int northWestTrackingSquare = SingleMoves::square_northWest(startSquare);
        const int maxNorthWestSteps = std::min(8- rank, file-1);
        for (int step = 0; step < maxNorthWestSteps; step++) {
            result |= 1ULL << northWestTrackingSquare;
            northWestTrackingSquare = SingleMoves::square_northWest(northWestTrackingSquare);
        }

        // go north east
        int northEastTrackingSquare = SingleMoves::square_northEast(startSquare);
        const int maxNorthEastSteps = std::min(8 - rank, 8 - file);
        for (int step = 0; step < maxNorthEastSteps; step++) {
            result |= 1ULL << northEastTrackingSquare;
            northEastTrackingSquare = SingleMoves::square_northEast(northEastTrackingSquare);
        }

        // go south east
        int southEastTrackingSquare = SingleMoves::square_southEast(startSquare);
        const int maxSouthEastSteps = std::min(rank - 1, 8 - file);
        for (int step = 0; step < maxSouthEastSteps; step++) {
            result |= 1ULL << southEastTrackingSquare;
            southEastTrackingSquare = SingleMoves::square_southEast(southEastTrackingSquare);
        }

        // go south west
        int southWestTrackingSquare = SingleMoves::square_southWest(startSquare);
        const int maxSouthWestSteps = std::min(rank - 1, file - 1);
        for (int step = 0; step < maxSouthWestSteps; step++) {
            result |= 1ULL << southWestTrackingSquare;
            southWestTrackingSquare = SingleMoves::square_southWest(southWestTrackingSquare);
        }

        return result;
    }
}

namespace RulesCheck {
    inline uint64_t getPsuedoLegalMoves(const int square, const Piece& piece){
        uint64_t result = 0ULL;
        switch (piece) {
            case WP:

                result |= SingleMoves::north(square);
                result |= SingleMoves::northEast(square);
                result |= SingleMoves::northWest(square);

                if (squareToRank(square) == 2) { result |= SingleMoves::north(SingleMoves::square_north(square)); }

                break;

            case BP:
                result |= SingleMoves::south(square);
                result |= SingleMoves::southEast(square);
                result |= SingleMoves::southWest(square);

                if (squareToRank(square) == 7) { result |= SingleMoves::south(SingleMoves::square_south(square)); }
                break;

            case WQ:
            case BQ:
                result |= CrossBoardMoves::diags(square);
                result |= CrossBoardMoves::wholeFile(square);
                result |= CrossBoardMoves::wholeRank(square);
                break;

            case WB:
            case BB:
                result |= CrossBoardMoves::diags(square);
                break;

            case WR:
            case BR:
                result |= CrossBoardMoves::wholeFile(square);
                result |= CrossBoardMoves::wholeRank(square);
                break;

            case WK:
            case BK:
                result |= SingleMoves::allDirs(square);
                break;

            case WN:
            case BN:
                result |= SingleMoves::getKnightMoves(square);
                break;


            default:
                break;
        }

        return result;
    }
}