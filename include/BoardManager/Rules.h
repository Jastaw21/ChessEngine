//
// Created by jacks on 25/06/2025.
//
#ifndef RULES_H
#define RULES_H

#include <algorithm>
#include <cstdint>

#include <bitset>
#include "BitBoards.h"
#include "Utility/math.h"

constexpr void buildRankAttacks(const int square, uint64_t& inBoard){
    const int rank = square / 8 + 1;

    // east then west
    const auto directions = {1, -1};

    for (const auto& dir: directions) {
        // bounds check
        const int cutoffFile = dir == 1 ? 8 : 1;
        for (int file = squareToFile(square) + dir; abs(file - cutoffFile) >= 0; file += dir) {
            if (file == 0 || file == 9)
                break;
            inBoard |= 1ULL << rankAndFileToSquare(rank, file);
        }
    }
}

constexpr void buildFileAttacks(const int square, uint64_t& inBoard){
    const int file = square % 8 + 1;

    // go north then south
    const auto directions = {1, -1};

    for (const auto& dir: directions) {
        // bounds check
        const int cutoffRank = dir == 1 ? 8 : 1;
        for (int rank = squareToRank(square) + dir; abs(rank - cutoffRank) >= 0; rank += dir) {
            if (rank == 0 || rank == 9)
                break;
            inBoard |= 1ULL << rankAndFileToSquare(rank, file);
        }
    }
}

class Rules {
public:

    Rules();
    uint64_t getAttackMoves(const int square, const Piece& piece, BitBoards* boards);
    uint64_t getPushMoves(const int square, const Piece& piece, BitBoards* boards);
    uint64_t getPseudoLegalMoves(const int square, const Piece& piece, BitBoards* bitBoards);
    uint64_t getCastlingMoves(const int square, const Piece& piece, BitBoards* boards);

    std::unordered_map<int, uint64_t> rankAttacks;
    std::unordered_map<int, uint64_t> fileAttacks;
};

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

    inline bool collisionOK(const int toSquare, BitBoards* boards, const bool isPush, const Colours& movingColour){
        if (isPush)
            return false; // if we're pushing, we can't move to any occupied square

        const auto collidedPiece = boards->getPiece(toSquare);
        if (collidedPiece.has_value()) {
            if (pieceColours[collidedPiece.value()] == movingColour)
                return false; // if the piece is friendly, we can't move there
        }
        return true;
    }

    inline uint64_t southChecked(const int square, BitBoards* boards, const bool isPush, const Colours& movingColour){
        if (squareToRank(square) == 1)
            return 0ULL; // can't go south from the first rank

        // check for collisions
        if (boards->testSquare(square_south(square))) {
            if (isPush)
                return 0ULL; // can't push south if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_south(square), boards, isPush, movingColour)) {
                return 1ULL << square_south(square);
            }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_south(square);
    }

    inline uint64_t northChecked(const int square, BitBoards* boards, const bool isPush, const Colours& movingColour){
        if (squareToRank(square) == 8)
            return 0ULL; // can't go north from the last rank

        // check for collisions
        if (boards->testSquare(square_north(square))) {
            if (isPush)
                return 0ULL; // can't push north if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_north(square), boards, isPush, movingColour)) {
                return 1ULL << square_north(square);
            }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_north(square);
    }

    inline uint64_t eastChecked(const int square, BitBoards* boards, const bool isPush, const Colours& movingColour){
        if (squareToFile(square) == 8)
            return 0ULL; // can't go east from the last rank

        // check for collisions
        if (boards->testSquare(square_east(square))) {
            if (isPush)
                return 0ULL; // can't push east if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_east(square), boards, isPush, movingColour)) { return 1ULL << square_east(square); }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_east(square);
    }

    inline uint64_t westChecked(const int square, BitBoards* boards, const bool isPush, const Colours& movingColour){
        if (squareToFile(square) == 1)
            return 0ULL; // can't go west from the first file

        // check for collisions
        if (boards->testSquare(square_west(square))) {
            if (isPush)
                return 0ULL; // can't push west if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_west(square), boards, isPush, movingColour)) { return 1ULL << square_west(square); }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_west(square);
    }

    inline uint64_t northEastChecked(const int square, BitBoards* boards, const bool isPush,
                                     const Colours& movingColour){
        if (squareToFile(square) == 8 || squareToRank(square) == 8)
            return 0ULL; // can't go northEast from last rank or last file

        // check for collisions
        if (boards->testSquare(square_northEast(square))) {
            if (isPush)
                return 0ULL; // can't push northEast if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_northEast(square), boards, isPush, movingColour)) {
                return 1ULL << square_northEast(square);
            }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_northEast(square);
    }

    inline uint64_t northWestChecked(const int square, BitBoards* boards, const bool isPush,
                                     const Colours& movingColour){
        if (squareToFile(square) == 1 || squareToRank(square) == 8)
            return 0ULL; // can't go northWest from first file or last rank

        // check for collisions
        if (boards->testSquare(square_northWest(square))) {
            if (isPush)
                return 0ULL; // can't push northWest if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_northWest(square), boards, isPush, movingColour)) {
                return 1ULL << square_northWest(square);
            }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_northWest(square);
    }

    inline uint64_t southEastChecked(const int square, BitBoards* boards, const bool isPush,
                                     const Colours& movingColour){
        if (squareToFile(square) == 8 || squareToRank(square) == 1)
            return 0ULL; // can't go southEast from the last file or first rank

        // check for collisions
        if (boards->testSquare(square_southEast(square))) {
            if (isPush)
                return 0ULL; // can't push southEast if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_southEast(square), boards, isPush, movingColour)) {
                return 1ULL << square_southEast(square);
            }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_southEast(square);
    }

    inline uint64_t southWestChecked(const int square, BitBoards* boards, const bool isPush,
                                     const Colours& movingColour){
        if (squareToFile(square) == 1 || squareToRank(square) == 1)
            return 0ULL; // can't go southWest from the first file or first rank

        // check for collisions
        if (boards->testSquare(square_southWest(square))) {
            if (isPush)
                return 0ULL; // can't push southWest if there's a piece in the way
            // otherwise, we can go there if it's an opponent
            if (collisionOK(square_southWest(square), boards, isPush, movingColour)) {
                return 1ULL << square_southWest(square);
            }
            return 0ULL; // otherwise, we can't go there
        }
        // no collision, all good to go there
        return 1ULL << square_southWest(square);
    }

    inline uint64_t oneSquareAllDirections(const int square){
        const uint64_t prelimLegalMoves = north(square) | south(square) | east(square)
                                          | west(square) | northEast(square) | northWest(square) |
                                          southEast(square) | southWest(square);

        return prelimLegalMoves;
    }

    inline uint64_t oneSquareAllDirections(const int square, BitBoards* boards, const bool isPush,
                                           const Colours& movingColour){
        const uint64_t prelimLegalMoves = north(square) | south(square)
                                          | east(square) | west(square)
                                          | northEast(square) | northWest(square)
                                          | southEast(square) | southWest(square);

        if (!boards->testBoard(prelimLegalMoves))
            return prelimLegalMoves;

        uint64_t occupiedSquares = 0ULL;
        for (int i = 0; i < PIECE_N; ++i) {
            const auto pieceName = static_cast<Piece>(i);
            if (isPush) {
                // any collision is a return
                const uint64_t collidedSquares = boards->getBitboard(pieceName);
                occupiedSquares |= collidedSquares;
            }

            // other wise, ony remove the friendly pieces
            else {
                if (pieceColours[pieceName] == movingColour) {
                    const uint64_t collidedSquares = boards->getBitboard(pieceName);
                    occupiedSquares |= collidedSquares;
                }
            }
        }

        return prelimLegalMoves & ~occupiedSquares;
    }


    inline uint64_t getKnightMoves(const int square, BitBoards* boards, const bool isPush,
                                   const Colours& movingColour){
        struct KnightMove {
            int rankOffset;
            int fileOffset;
        };

        // All possible knight moves patterns
        constexpr KnightMove knightMovePatterns[] = {
                    {2, -1}, {2, 1}, // Up 2, left/right 1
                    {1, -2}, {1, 2}, // Up 1, left/right 2
                    {-1, -2}, {-1, 2}, // Down 1, left/right 2
                    {-2, -1}, {-2, 1} // Down 2, left/right 1
                };

        uint64_t result = 0ULL;
        const int currentRank = squareToRank(square);
        const int currentFile = squareToFile(square);

        for (const auto& move: knightMovePatterns) {
            const int targetRank = currentRank + move.rankOffset;
            const int targetFile = currentFile + move.fileOffset;

            if (!(targetFile >= 1 && targetFile <= 8))
                continue; // Check if the target position is within the board boundaries
            if (!(targetRank >= 1 && targetRank <= 8))
                continue;
            // check if we collide with anything
            const auto collidedPiece = boards->getPiece(rankAndFileToSquare(targetRank, targetFile));
            if (collidedPiece.has_value()) {
                if (isPush)
                    continue; // any collision while pushing shouldn't be possible

                // otherwise, check the colour of the collided piece
                if (pieceColours[collidedPiece.value()] == movingColour)
                    continue;

                result |= 1ULL << rankAndFileToSquare(targetRank, targetFile);
            }

            result |= 1ULL << rankAndFileToSquare(targetRank, targetFile);
        }

        return result;
    }
}

namespace Sliders {
    inline uint64_t wholeFile(const int startSquare){
        uint64_t result = 0ULL;
        const int file = squareToFile(startSquare);
        for (int rank = 1; rank <= 8; rank++) {
            if (rank == squareToRank(startSquare))
                continue; // skip the start square
            result |= 1ULL << rankAndFileToSquare(rank, file);
        }
        return result;
    }

    inline uint64_t wholeFile(const int startSquare, const BitBoards* bitBoards, const bool isPush,
                              const Colours& movingColour){
        uint64_t result = 0ULL;
        const int file = squareToFile(startSquare);

        // go north, then south
        const auto directions = {1, -1};
        for (const auto& dir: directions) {
            // bounds check
            const int cutoffRank = dir == 1 ? 8 : 1;
            for (int rank = squareToRank(startSquare) + dir; abs(rank - cutoffRank) >= 0; rank += dir) {
                if (rank == 0 || rank == 9)
                    break;

                // see if any collisions
                if (bitBoards->testSquare(rankAndFileToSquare(rank, file))) {
                    if (isPush)
                        break;

                    // otherwise, we're checking if it's friendly or enemy
                    const auto collidedPiece = bitBoards->getPiece(rankAndFileToSquare(rank, file));

                    // friendly case, just return
                    if (pieceColours[collidedPiece.value()] == movingColour) { break; }

                    // opponent case, move to that square and exit

                    result |= 1ULL << rankAndFileToSquare(rank, file);
                    break; // then duck out
                }

                // no collision, add the square and continue

                result |= 1ULL << rankAndFileToSquare(rank, file);
            }
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

    inline uint64_t wholeRank(const int startSquare, const BitBoards* bitBoards, const bool isPush,
                              const Colours& movingColour){
        uint64_t result = 0ULL;
        const int rank = squareToRank(startSquare);

        // east then west
        const auto directions = {1, -1};

        for (const auto& dir: directions) {
            // bounds check
            const int cutoffFile = dir == 1 ? 8 : 1;
            for (int file = squareToFile(startSquare) + dir; abs(file - cutoffFile) >= 0; file += dir) {
                if (file == 0 || file == 9)
                    break;

                // see if any collisions
                if (bitBoards->testSquare(rankAndFileToSquare(rank, file))) {
                    if (isPush)
                        break;

                    // otherwise, we're checking if it's friendly or enemy
                    const auto collidedPiece = bitBoards->getPiece(rankAndFileToSquare(rank, file));

                    // friendly case, just return
                    if (pieceColours[collidedPiece.value()] == movingColour) { break; }

                    // opponent case, move to that square and exit

                    result |= 1ULL << rankAndFileToSquare(rank, file);
                    break; // then duck out
                }

                // no collision, add the square and continue
                result |= 1ULL << rankAndFileToSquare(rank, file);
            }
        }

        return result;
    }

    inline uint64_t diags(const int startSquare){
        uint64_t result = 0ULL;
        int rank, file;
        squareToRankAndFile(startSquare, rank, file);

        // go north-west
        int nextSquareNW = SingleMoves::square_northWest(startSquare);
        const int stepsNW = std::min(8 - rank, file - 1);
        for (int step = 0; step < stepsNW; step++) {
            result |= 1ULL << nextSquareNW;
            nextSquareNW = SingleMoves::square_northWest(nextSquareNW);
        }

        // go north-east
        int northEastTrackingSquare = SingleMoves::square_northEast(startSquare);
        const int maxNorthEastSteps = std::min(8 - rank, 8 - file);
        for (int step = 0; step < maxNorthEastSteps; step++) {
            result |= 1ULL << northEastTrackingSquare;
            northEastTrackingSquare = SingleMoves::square_northEast(northEastTrackingSquare);
        }

        // go south-east
        int southEastTrackingSquare = SingleMoves::square_southEast(startSquare);
        const int maxSouthEastSteps = std::min(rank - 1, 8 - file);
        for (int step = 0; step < maxSouthEastSteps; step++) {
            result |= 1ULL << southEastTrackingSquare;
            southEastTrackingSquare = SingleMoves::square_southEast(southEastTrackingSquare);
        }

        // go south-west
        int southWestTrackingSquare = SingleMoves::square_southWest(startSquare);
        const int maxSouthWestSteps = std::min(rank - 1, file - 1);
        for (int step = 0; step < maxSouthWestSteps; step++) {
            result |= 1ULL << southWestTrackingSquare;
            southWestTrackingSquare = SingleMoves::square_southWest(southWestTrackingSquare);
        }

        return result;
    }

    inline uint64_t diags(const int startSquare, const BitBoards* bitBoards, const bool isPush,
                          const Colours& movingColour){
        uint64_t result = 0ULL;
        int rank, file;
        squareToRankAndFile(startSquare, rank, file);

        // go north-west
        int squareNW = SingleMoves::square_northWest(startSquare); // get the first square
        const int stepsNW = std::min(8 - rank, file - 1); // how far can we go before falling off the baord
        for (int step = 0; step < stepsNW; step++) {
            if (bitBoards->testSquare(squareNW)) {
                // if we find a piece, break immediately if it's a friendly piece, move once more if enemy
                if (isPush) { break; }
                const auto collidedPiece = bitBoards->getPiece(squareNW);
                // if the piece is friendly, just break
                if (pieceColours[collidedPiece.value()] == movingColour)
                    break;
                // otherwise, advance a square before breaking
                result |= 1ULL << squareNW;
                break;
            }
            // no collision found, add the square to the result and move forwards
            result |= 1ULL << squareNW;
            squareNW = SingleMoves::square_northWest(squareNW);
        }

        // go north-east
        int squareNE = SingleMoves::square_northEast(startSquare);
        const int stepsNE = std::min(8 - rank, 8 - file);
        for (int step = 0; step < stepsNE; step++) {
            if (bitBoards->testSquare(squareNE)) {
                // if we find a piece, break immediately if it's a friendly piece, move once more if enemy
                if (isPush) { break; }
                const auto collidedPiece = bitBoards->getPiece(squareNE);
                // if the piece is friendly, just break
                if (pieceColours[collidedPiece.value()] == movingColour)
                    break;
                // otherwise, advance a square before breaking
                result |= 1ULL << squareNE;
                break;
            }
            // no collision found, add the square to the result and move forwards
            result |= 1ULL << squareNE;
            squareNE = SingleMoves::square_northEast(squareNE);
        }

        // go south-east
        int squareSE = SingleMoves::square_southEast(startSquare);
        const int stepsSE = std::min(rank - 1, 8 - file);
        for (int step = 0; step < stepsSE; step++) {
            if (bitBoards->testSquare(squareSE)) {
                // if we find a piece, break immediately if it's a friendly piece, move once more if enemy
                if (isPush) { break; }
                const auto collidedPiece = bitBoards->getPiece(squareSE);
                // if the piece is friendly, just break
                if (pieceColours[collidedPiece.value()] == movingColour)
                    break;
                // otherwise, advance a square before breaking
                result |= 1ULL << squareSE;
                break;
            }
            // no collision found, add the square to the result and move forwards
            result |= 1ULL << squareSE;
            squareSE = SingleMoves::square_southEast(squareSE);
        }

        // go south-west
        int squareSW = SingleMoves::square_southWest(startSquare);
        const int stepsSW = std::min(rank - 1, file - 1);
        for (int step = 0; step < stepsSW; step++) {
            if (bitBoards->testSquare(squareSW)) {
                // if we find a piece, break immediately if it's a friendly piece, move once more if enemy
                if (isPush) { break; }
                const auto collidedPiece = bitBoards->getPiece(squareSW);
                // if the piece is friendly, just break
                if (pieceColours[collidedPiece.value()] == movingColour)
                    break;
                // otherwise, advance a square before breaking
                result |= 1ULL << squareSW;
                break;
            }
            // no collision found, add the square to the result and move forwards
            result |= 1ULL << squareSW;
            squareSW = SingleMoves::square_southWest(squareSW);
        }

        return result;
    }
}

namespace RulesCheck {
    inline uint64_t getPushMoves(const int square, const Piece& piece, BitBoards* boards){
        uint64_t result = 0ULL;
        switch (piece) {
            case WP:

                // default single north move
                result |= SingleMoves::northChecked(square, boards, true, pieceColours[piece]);

                // if we're on the starting rank, can move two
                if (squareToRank(square) == 2) {
                    // can't jump a piece
                    if (boards->testSquare(SingleMoves::square_north(square)))
                        break;
                    // can't go to an occupied square
                    if (boards->testSquare(SingleMoves::square_north(SingleMoves::square_north(square))))
                        break;

                    // otherwise can go two north
                    result |= SingleMoves::north(SingleMoves::square_north(square));
                }
                break;

            case BP:
                // default single south move
                result |= SingleMoves::southChecked(square, boards, true, pieceColours[piece]);

                // if we're on the starting rank, can move two
                if (squareToRank(square) == 7) {
                    // can't jump a piece
                    if (boards->testSquare(SingleMoves::square_south(square)))
                        break;
                    // can't go to an occupied square
                    if (boards->testSquare(SingleMoves::square_south(SingleMoves::square_south(square))))
                        break;

                    // otherwise can go two south
                    result |= SingleMoves::south(SingleMoves::square_south(square));
                }
                break;
            case WQ:
            case BQ:
                result |= Sliders::diags(square, boards, true, pieceColours[piece]);
                result |= Sliders::wholeFile(square, boards, true, pieceColours[piece]);
                result |= Sliders::wholeRank(square, boards, true, pieceColours[piece]);
                break;

            case WB:
            case BB:
                result |= Sliders::diags(square, boards, true, pieceColours[piece]);
                break;

            case WR:
            case BR:
                result |= Sliders::wholeFile(square, boards, true, pieceColours[piece]);
                result |= Sliders::wholeRank(square, boards, true, pieceColours[piece]);
                break;

            case WK:
            case BK:
                result |= SingleMoves::oneSquareAllDirections(square, boards, true, pieceColours[piece]);
                break;

            case WN:
            case BN:
                result |= SingleMoves::getKnightMoves(square, boards, true, pieceColours[piece]);
                break;

            default:
                break;
        }
        return result;
    }

    inline uint64_t getAttackMoves(const int square, const Piece& piece, BitBoards* boards){
        uint64_t result = 0ULL;
        switch (piece) {
            case WP:
                if (!boards->testSquare(SingleMoves::square_north(square)))
                    result |= SingleMoves::north(square);

                if (squareToRank(square) == 2 && !boards->testSquare(
                        SingleMoves::square_north(SingleMoves::square_north(square))))
                    result |= SingleMoves::north(SingleMoves::square_north(square));

                result |= SingleMoves::northEastChecked(square, boards, false, pieceColours[piece]);
                result |= SingleMoves::northWestChecked(square, boards, false, pieceColours[piece]);
                break;
            case BP:
                if (!boards->testSquare(SingleMoves::square_south(square)))
                    result |= SingleMoves::south(square);

                if (squareToRank(square) == 7 && !boards->testSquare(
                        SingleMoves::square_south(SingleMoves::square_south(square))))
                    result |= SingleMoves::south(SingleMoves::square_south(square));
                result |= SingleMoves::southEastChecked(square, boards, false, pieceColours[piece]);
                result |= SingleMoves::southWestChecked(square, boards, false, pieceColours[piece]);
                break;

            case WQ:
            case BQ:
                result |= Sliders::diags(square, boards, false, pieceColours[piece]);
                result |= Sliders::wholeFile(square, boards, false, pieceColours[piece]);
                result |= Sliders::wholeRank(square, boards, false, pieceColours[piece]);
                break;

            case WB:
            case BB:
                result |= Sliders::diags(square, boards, false, pieceColours[piece]);
                break;

            case WR:
            case BR:
                result |= Sliders::wholeFile(square, boards, false, pieceColours[piece]);
                result |= Sliders::wholeRank(square, boards, false, pieceColours[piece]);
                break;

            case WK:
            case BK:
                result |= SingleMoves::oneSquareAllDirections(square, boards, false, pieceColours[piece]);
                break;

            case WN:
            case BN:
                result |= SingleMoves::getKnightMoves(square, boards, false, pieceColours[piece]);
                break;

            default:
                break;
        }

        return result;
    }

    inline uint64_t getCastlingMoves(const int square, const Piece& piece, BitBoards* boards){
        // only kings
        if (piece != WK && piece != BK)
            return 0ULL;

        int fileFrom, rankFrom;
        squareToRankAndFile(square, rankFrom, fileFrom);

        // has to be from king start pos
        if (fileFrom != 5)
            return 0ULL;
        // only works on rank 1 if white
        if (piece == WK && rankFrom != 1)
            return 0ULL;
        // only works on rank 8 if black
        if (piece == BK && rankFrom != 8)
            return 0ULL;

        const auto destinationFiles = {3, 7};
        uint64_t result = 0ULL;
        for (const auto& file: destinationFiles) {
            if (boards->testSquare(rankAndFileToSquare(rankFrom, file))) {
                continue; // if there's a piece here, just continue on to the next direction
            }

            // also can't go there if we cross pieces
            const int deltaFile = file - fileFrom;
            bool canCastle = true;

            for (int intermediateFile = fileFrom + sign(deltaFile); intermediateFile != file;
                 intermediateFile += sign(deltaFile)) {
                if (boards->testSquare(rankAndFileToSquare(rankFrom, intermediateFile))) {
                    canCastle = false;
                    break; // if we cross anything on the way, break out and don't continue checking the others
                }

                uint64_t attackedSquares = 0ULL;
                // also check if it's attacked by anything
                for (int i = 0; i < PIECE_N; ++i) {
                    const auto pieceName = static_cast<Piece>(i);
                    if (pieceColours[pieceName] == pieceColours[piece])
                        continue;

                    auto startingBits = std::bitset<64>(boards->getBitboard(pieceName));
                    for (int index = 0; index < startingBits.size(); ++index) {
                        if (startingBits.test(index)) {
                            const uint64_t attacks = getAttackMoves(index, pieceName, boards);
                            const uint64_t pushes = getPushMoves(index, pieceName, boards);
                            const uint64_t allMoves = attacks | pushes;
                            attackedSquares |= allMoves;
                            if (attackedSquares & 1ULL << rankAndFileToSquare(rankFrom, intermediateFile)) {
                                canCastle = false;
                                break;
                            }
                        }
                    }
                }
                if (attackedSquares & 1ULL << rankAndFileToSquare(rankFrom, intermediateFile)) {
                    canCastle = false;
                    break;
                }
            }
            // if we can castle, add the move
            if (canCastle)
                result |= 1ULL << rankAndFileToSquare(rankFrom, file);
        }

        return result;
    }

    inline uint64_t getPseudoLegalMoves(const int square, const Piece& piece, BitBoards* bitBoards){
        uint64_t result = 0ULL;
        result |= getPushMoves(square, piece, bitBoards);
        result |= getAttackMoves(square, piece, bitBoards);
        if (piece == WK || piece == BK)
            result |= getCastlingMoves(square, piece, bitBoards);
        return result;
    }

    inline uint64_t getEnPassantVulnerableSquares(BitBoards* boards, const Colours& moveColour){
        uint64_t result = 0ULL;
        for (int piece = 0; piece < PIECE_N; ++piece) {
            const auto pieceName = static_cast<Piece>(piece);

            // don't care about own colour
            if (pieceColours[pieceName] == moveColour)
                continue;
            // only care about pawns
            if (pieceName != WP && BP != pieceName)
                continue;

            const bool isWhite = pieceName == WP;
            const int squareOffset = isWhite ? -8 : 8;

            const auto& otherPawnLocations = std::bitset<64>(boards->getBitboard(pieceName));

            for (int square = 0; square < otherPawnLocations.size(); ++square) {
                // no opponent pawns here
                if (!otherPawnLocations.test(square))
                    continue;
                // is the square occupied
                if (boards->testSquare(square + squareOffset))
                    continue;

                // en passant can only happen on 4 or 5
                if (squareToRank(square) != 5 && squareToRank(square) != 4)
                    continue;

                if (otherPawnLocations.test(square) && !boards->testSquare(square + squareOffset))
                    result |= 1ULL << (square + squareOffset);
            }
        }

        return result;
    }
}
#endif // RULES_H