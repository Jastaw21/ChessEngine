//
// Created by jacks on 25/06/2025.
//
#ifndef RULES_H
#define RULES_H

#include <algorithm>

#include <bitset>
#include "BitBoards.h"
#include "Utility/math.h"
#include <bit>

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

    void getPseudoAttacks(const Piece& piece, const int fromSquare, Bitboard& inBoard){
        switch (piece) {
            case BP:
            case WP: {
                inBoard |= getPseudoPawnAttacks(piece, fromSquare);
                break;
            }
            case WN:
            case BN: {
                inBoard |= knightAttacks[fromSquare];
                break;
            }
            case WQ:
            case BQ: {
                inBoard |= (rankAttacks[fromSquare] | fileAttacks[fromSquare] | diagAttacks[fromSquare]);
                break;
            }
            case WB:
            case BB: {
                inBoard |= diagAttacks[fromSquare];
                break;
            }
            case WR:
            case BR: {
                inBoard |= (rankAttacks[fromSquare] | fileAttacks[fromSquare]);
                break;
            }

            case WK:
            case BK: {
                inBoard |= kingMoves[fromSquare];
                break;
            }
            default: {
                inBoard = 0ULL;
                break;
            }
        }
    }

    Bitboard getPseudoCastlingMoves(const Piece& piece, const int fromSquare, const BitBoards& boards);

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
    inline void southInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL >> 8; }
    inline void northInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL << 8; }
    inline void eastInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL >> 1; }
    inline void westInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL << 1; }
    inline void northEastInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL >> 9; }
    inline void northWestInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL >> 7; }
    inline void southEastInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL << 7; }
    inline void southWestInPlace(Bitboard& inBitBoard){ inBitBoard &= 1ULL << 9; }

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

namespace Sliders {
    inline Bitboard wholeFile(const int startSquare){
        Bitboard result = 0ULL;
        const int file = squareToFile(startSquare);
        for (int rank = 1; rank <= 8; rank++) {
            if (rank == squareToRank(startSquare))
                continue; // skip the start square
            result |= 1ULL << rankAndFileToSquare(rank, file);
        }
        return result;
    }

    inline Bitboard wholeFile(const int startSquare, const BitBoards* bitBoards, const bool isPush,
                              const Colours& movingColour){
        Bitboard result = 0ULL;
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

    inline Bitboard wholeRank(const int startSquare){
        Bitboard result = 0ULL;
        const int rank = squareToRank(startSquare);
        for (int file = 1; file <= 8; file++) {
            if (file == squareToFile(startSquare))
                continue;
            result |= 1ULL << rankAndFileToSquare(rank, file);
        }
        return result;
    }

    inline Bitboard wholeRank(const int startSquare, const BitBoards* bitBoards, const bool isPush,
                              const Colours& movingColour){
        Bitboard result = 0ULL;
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

    inline Bitboard diags(const int startSquare){
        Bitboard result = 0ULL;
        int rank, file;
        squareToRankAndFile(startSquare, rank, file);

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
                result |= 1ULL << rankAndFileToSquare(rank + move.dRank * step, file + move.dFile * step);
            }
        }
        return result;
    }

    inline Bitboard diags(const int startSquare, const BitBoards* bitBoards, const bool isPush,
                          const Colours& movingColour){
        Bitboard result = 0ULL;
        int rank, file;
        squareToRankAndFile(startSquare, rank, file);

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
                const auto square = rankAndFileToSquare(rank + move.dRank * step, file + move.dFile * step);
                if (bitBoards->testSquare(square)) {
                    if (isPush)
                        break;
                    // otherwise, we're checking if it's friendly or enemy
                    const auto collidedPiece = bitBoards->getPiece(square);

                    // friendly case, just return
                    if (pieceColours[collidedPiece.value()] == movingColour) { break; }

                    // opponent case, move to that square and exit

                    result |= 1ULL << square;
                    break; // then duck out
                }
                result |= 1ULL << square;
            }
        }

        return result;
    }
}

namespace RulesCheck {
    inline Bitboard getPushMoves(const int square, const Piece& piece, const BitBoards* boards){
        Bitboard result = 0ULL;
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

                    // otherwise can go two squares north
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

                    // otherwise can go two squares south
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

    inline Bitboard getEnPassantVulnerableSquares(const BitBoards* boards, const Colours& moveColour){
        Bitboard result = 0ULL;
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

            auto startingPawnLocations = boards->getBitboard(pieceName);

            while (startingPawnLocations) {
                // count trailing zeros to find the index of the first set bit
                const int square = std::countr_zero(startingPawnLocations);
                startingPawnLocations &= ~(1ULL << square); // pop this bit off

                // is the square occupied?
                if (boards->testSquare(square + squareOffset))
                    continue;

                // en passant can only happen on 4 or 5
                if (squareToRank(square) != 5 && squareToRank(square) != 4)
                    continue;

                if (!boards->testSquare(square + squareOffset))
                    result |= 1ULL << (square + squareOffset);
            }
        }

        return result;
    }

    inline Bitboard getAttackMoves(const int square, const Piece& piece, const BitBoards* boards){
        Bitboard result = 0ULL;
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

        const auto otherPiece = pieceColours[piece] == WHITE ? BLACK : WHITE;
        return result & (boards->getOccupancy(otherPiece) | getEnPassantVulnerableSquares(boards, pieceColours[piece]));
    }

    inline Bitboard getCastlingMoves(const int square, const Piece& piece, const BitBoards* boards){
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
        Bitboard result = 0ULL;
        for (const auto& file: destinationFiles) {
            if (boards->testSquare(rankAndFileToSquare(rankFrom, file))) {
                continue; // if there's a piece here, continue on to the next direction
            }

            // also can't go there if we cross pieces
            const int deltaFile = file - fileFrom;
            bool canCastle = true;

            for (int intermediateFile = fileFrom + MathUtility::sign(deltaFile); intermediateFile != file;
                 intermediateFile += MathUtility::sign(deltaFile)) {
                if (boards->testSquare(rankAndFileToSquare(rankFrom, intermediateFile))) {
                    canCastle = false;
                    break; // if we cross anything on the way, break out and don't continue checking the others
                }

                Bitboard attackedSquares = 0ULL;
                // also check if it's attacked by anything
                for (int i = 0; i < PIECE_N; ++i) {
                    const auto pieceName = static_cast<Piece>(i);
                    if (pieceColours[pieceName] == pieceColours[piece])
                        continue;

                    auto startingBits = std::bitset<64>(boards->getBitboard(pieceName));
                    for (int index = 0; index < startingBits.size(); ++index) {
                        if (startingBits.test(index)) {
                            const Bitboard attacks = getAttackMoves(index, pieceName, boards);
                            const Bitboard pushes = getPushMoves(index, pieceName, boards);
                            const Bitboard allMoves = attacks | pushes;
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

    inline Bitboard getPseudoLegalMoves(const int square, const Piece& piece, const BitBoards* bitBoards){
        Bitboard result = 0ULL;
        result |= getPushMoves(square, piece, bitBoards);
        result |= getAttackMoves(square, piece, bitBoards);
        if (piece == WK || piece == BK)
            result |= getCastlingMoves(square, piece, bitBoards);
        return result;
    }
}
#endif // RULES_H