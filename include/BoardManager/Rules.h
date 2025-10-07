//
// Created by jacks on 25/06/2025.
//
#ifndef RULES_H
#define RULES_H

#include <algorithm>

#include <bitset>
#include "BitBoards.h"
#include "Utility/math.h"

constexpr void buildRankAttacks(const int square, Bitboard& inBoard){
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

constexpr void buildFileAttacks(const int square, Bitboard& inBoard){
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

constexpr void buildDiagonalAttacks(const int square, Bitboard& inBoard){
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;

    struct DiagonalMove {
        int dRank;
        int dFile;
    };

    constexpr DiagonalMove diagonalMovePatterns[] = {
                {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
            };

    for (const auto& move: diagonalMovePatterns) {
        const int fileCutoff = move.dFile == 1 ? 8 : 1;
        const int rankCutoff = move.dRank == 1 ? 8 : 1;
        const int steps = std::min(abs(file - fileCutoff), abs(rank - rankCutoff));

        for (int step = 1; step <= steps; ++step) {
            inBoard |= 1ULL << rankAndFileToSquare(rank + move.dRank * step, file + move.dFile * step);
        }
    }
}

constexpr void buildKnightAttacks(const int square, Bitboard& inBoard){
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
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;

    for (const auto& move: knightMovePatterns) {
        const int targetRank = rank + move.rankOffset;
        const int targetFile = file + move.fileOffset;

        if (!(targetFile >= 1 && targetFile <= 8))
            continue; // Check if the target position is within the board boundaries
        if (!(targetRank >= 1 && targetRank <= 8))
            continue;
        inBoard |= 1ULL << rankAndFileToSquare(targetRank, targetFile);
    }
}

constexpr void buildWhitePawnPushes(const int square, Bitboard& inBoard){
    const int rank = square / 8 + 1;
    if (rank != 8)
        inBoard |= 1ULL << square + 8;
    if (rank == 2)
        inBoard |= 1ULL << square + 16;
}

constexpr void buildBlackPawnPushes(const int square, Bitboard& inBoard){
    const int rank = square / 8 + 1;
    if (rank != 1)
        inBoard |= 1ULL << square - 8;
    if (rank == 7)
        inBoard |= 1ULL << square - 16;
}

constexpr void buildWhitePawnAttacks(const int square, Bitboard& inBoard){
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;
    if (rank != 8) {
        if (file != 1)
            inBoard |= 1ULL << square + 7;
        if (file != 8)
            inBoard |= 1ULL << square + 9;
    }
}

constexpr void buildBlackPawnAttacks(const int square, Bitboard& inBoard){
    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;
    if (rank != 1) {
        if (file != 1)
            inBoard |= 1ULL << square - 9;
        if (file != 8)
            inBoard |= 1ULL << square - 7;
    }
}

constexpr void buildKingMoves(const int square, Bitboard& inBoard){
    struct KingDirections {
        int dRank;
        int dFile;
    };
    constexpr KingDirections knightMovePatterns[] = {
                {-1, -1}, {-1, 0}, {-1, 1}, // Up 2, left/right 1
                {0, -1}, {0, 1},
                {1, -1}, {1, 0}, {1, 1}
            };

    const int rank = square / 8 + 1;
    const int file = square % 8 + 1;

    for (const auto& move: knightMovePatterns) {
        const int targetRank = rank + move.dRank;
        const int targetFile = file + move.dFile;
        if (targetRank >= 1 && targetRank <= 8 && targetFile >= 1 && targetFile <= 8)
            inBoard |= 1ULL << rankAndFileToSquare(targetRank, targetFile);
    }
}

class Rules {
public:

    Rules();

    Bitboard getPseudoPawnEP(const Piece& piece, const int fromSquare, const Bitboard& opponentPawnOccupancy){
        Bitboard opponentPawnStarts = piece == WP ? 0xff00000000 : 0xff000000;

        // need to check if the opponent pawns are even in the right starting places
        if ((opponentPawnOccupancy & opponentPawnStarts) == 0)
            return 0ULL;

        // must end up north or south (if black) of the other pawns
        if (piece == WP) { opponentPawnStarts <<= 8; } else { opponentPawnStarts >>= 8; }

        auto allValidEnPassantTargetSquares = opponentPawnStarts;
        return getPseudoPawnAttacks(piece, fromSquare) & allValidEnPassantTargetSquares;
    }

    Bitboard getPseudoPawnPushes(const Piece& piece, const int fromSquare){
        if (piece == WP) { return whitePawnPushes[fromSquare]; }
        if (piece == BP) { return blackPawnPushes[fromSquare]; }
        return 0ULL;
    }

    Bitboard getPseudoPawnAttacks(const Piece& piece, const int fromSquare){
        if (piece == WP) { return whitePawnAttacks[fromSquare]; }
        if (piece == BP) { return blackPawnAttacks[fromSquare]; }
        return 0ULL;
    }

    Bitboard getPseudoAttacks(const Piece& piece, const int fromSquare){
        switch (piece) {
            case BP:
            case WP:
                return getPseudoPawnAttacks(piece, fromSquare);
            case WN:
            case BN:
                return knightAttacks[fromSquare];
            case WQ:
            case BQ:
                return rankAttacks[fromSquare] | fileAttacks[fromSquare] | diagAttacks[fromSquare];
            case WB:
            case BB:
                return diagAttacks[fromSquare];
            case WR:
            case BR:
                return rankAttacks[fromSquare] | fileAttacks[fromSquare];

            case WK:
            case BK:
                return kingMoves[fromSquare];
            default:
                return 0ULL;
        }
    }

    std::array<Bitboard, 64> rankAttacks;
    std::array<Bitboard, 64> fileAttacks;
    std::array<Bitboard, 64> diagAttacks;
    std::array<Bitboard, 64> knightAttacks;
    std::array<Bitboard, 64> whitePawnPushes;
    std::array<Bitboard, 64> whitePawnAttacks;
    std::array<Bitboard, 64> blackPawnPushes;
    std::array<Bitboard, 64> blackPawnAttacks;
    std::array<Bitboard, 64> kingMoves;
};

namespace SingleMoves {
    inline int square_south(const int square){ return square - 8; }
    inline int square_north(const int square){ return square + 8; }
    inline int square_east(const int square){ return square + 1; }
    inline int square_west(const int square){ return square - 1; }
    inline int square_northEast(const int square){ return square + 9; }
    inline int square_northWest(const int square){ return square + 7; }
    inline int square_southEast(const int square){ return square - 7; }
    inline int square_southWest(const int square){ return square - 9; }


    inline Bitboard south(const int square){
        if (squareToRank(square) == 1)
            return 0ULL;
        return 1ULL << square_south(square);
    }

    inline Bitboard north(const int square){
        if (squareToRank(square) == 8)
            return 0ULL;
        return 1ULL << square_north(square);
    }

    inline Bitboard east(const int square){
        if (squareToFile(square) == 8)
            return 0ULL;
        return 1ULL << square_east(square);
    }

    inline Bitboard west(const int square){
        if (squareToFile(square) == 1)
            return 0ULL;
        return 1ULL << square_west(square);
    }

    inline Bitboard northEast(const int square){
        if (squareToFile(square) == 8 || squareToRank(square) == 8)
            return 0ULL;
        return 1ULL << square_northEast(square);
    }

    inline Bitboard northWest(const int square){
        if (squareToFile(square) == 1 || squareToRank(square) == 8)
            return 0ULL;
        return 1ULL << square_northWest(square);
    }

    inline Bitboard southEast(const int square){
        if (squareToFile(square) == 8 || squareToRank(square) == 1)
            return 0ULL;
        return 1ULL << square_southEast(square);
    }

    inline Bitboard southWest(const int square){
        if (squareToFile(square) == 1
            || squareToRank(square) == 1)
            return 0ULL;
        return 1ULL << square_southWest(square);
    }

    inline bool collisionOK(const int toSquare, const BitBoards* boards, const bool isPush,
                            const Colours& movingColour){
        if (isPush)
            return false; // if we're pushing, we can't move to any occupied square

        const auto collidedPiece = boards->getPiece(toSquare);
        if (collidedPiece.has_value()) {
            if (pieceColours[collidedPiece.value()] == movingColour)
                return false; // if the piece is friendly, we can't move there
        }
        return true;
    }

    inline Bitboard southChecked(const int square, const BitBoards* boards, const bool isPush,
                                 const Colours& movingColour){
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

    inline Bitboard northChecked(const int square, const BitBoards* boards, const bool isPush,
                                 const Colours& movingColour){
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

    inline Bitboard eastChecked(const int square, const BitBoards* boards, const bool isPush,
                                const Colours& movingColour){
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

    inline Bitboard westChecked(const int square, const BitBoards* boards, const bool isPush,
                                const Colours& movingColour){
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

    inline Bitboard northEastChecked(const int square, const BitBoards* boards, const bool isPush,
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

    inline Bitboard northWestChecked(const int square, const BitBoards* boards, const bool isPush,
                                     const Colours& movingColour){
        if (squareToFile(square) == 1 || squareToRank(square) == 8)
            return 0ULL; // can't go northWest from the first file or last rank

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

    inline Bitboard southEastChecked(const int square, const BitBoards* boards, const bool isPush,
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

    inline Bitboard southWestChecked(const int square, const BitBoards* boards, const bool isPush,
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

    inline Bitboard oneSquareAllDirections(const int square){
        const Bitboard prelimLegalMoves = north(square) | south(square) | east(square)
                                          | west(square) | northEast(square) | northWest(square) |
                                          southEast(square) | southWest(square);

        return prelimLegalMoves;
    }

    inline Bitboard oneSquareAllDirections(const int square, const BitBoards* boards, const bool isPush,
                                           const Colours& movingColour){
        const Bitboard prelimLegalMoves = north(square) | south(square)
                                          | east(square) | west(square)
                                          | northEast(square) | northWest(square)
                                          | southEast(square) | southWest(square);

        if (!boards->testBoard(prelimLegalMoves))
            return prelimLegalMoves;

        Bitboard occupiedSquares = 0ULL;
        for (int i = 0; i < PIECE_N; ++i) {
            const auto pieceName = static_cast<Piece>(i);
            if (isPush) {
                // any collision is a return
                const Bitboard collidedSquares = boards->getBitboard(pieceName);
                occupiedSquares |= collidedSquares;
            }

            // otherwise, ony remove the friendly pieces
            else {
                if (pieceColours[pieceName] == movingColour) {
                    const Bitboard collidedSquares = boards->getBitboard(pieceName);
                    occupiedSquares |= collidedSquares;
                }
            }
        }

        return prelimLegalMoves & ~occupiedSquares;
    }


    inline Bitboard getKnightMoves(const int square, const BitBoards* boards, const bool isPush,
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

        Bitboard result = 0ULL;
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


#endif // RULES_H