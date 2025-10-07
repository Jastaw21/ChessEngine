//
// Created by jacks on 06/07/2025.
//
#include "BoardManager/Rules.h"

Rules::Rules(){
    for (int square = 0; square < 64; square++) {
        Bitboard rankResult = 0ULL;
        buildRankAttacks(square, rankResult);
        rankAttacks[square] = rankResult;

        Bitboard fileResult = 0ULL;
        buildFileAttacks(square, fileResult);
        fileAttacks[square] = fileResult;

        Bitboard diagResult = 0ULL;
        buildDiagonalAttacks(square, diagResult);
        diagAttacks[square] = diagResult;

        Bitboard knightResult = 0ULL;
        buildKnightAttacks(square, knightResult);
        knightAttacks[square] = knightResult;

        Bitboard whitePawnPushResult = 0ULL;
        buildWhitePawnPushes(square, whitePawnPushResult);
        whitePawnPushes[square] = whitePawnPushResult;

        Bitboard blackPawnPushResult = 0ULL;
        buildBlackPawnPushes(square, blackPawnPushResult);
        blackPawnPushes[square] = blackPawnPushResult;

        Bitboard whitePawnAttackResult = 0ULL;
        buildWhitePawnAttacks(square, whitePawnAttackResult);
        whitePawnAttacks[square] = whitePawnAttackResult;

        Bitboard blackPawnAttackResult = 0ULL;
        buildBlackPawnAttacks(square, blackPawnAttackResult);
        blackPawnAttacks[square] = blackPawnAttackResult;

        Bitboard kingPushResult = 0ULL;
        buildKingMoves(square, kingPushResult);
        kingMoves[square] = kingPushResult;
    }
}

Bitboard Rules::getPseudoPawnEP(const Piece& piece, const int fromSquare, const Bitboard& opponentPawnOccupancy){
    Bitboard opponentPawnStarts = piece == WP ? 0xff00000000 : 0xff000000;

    // need to check if the opponent pawns are even in the right starting places
    if ((opponentPawnOccupancy & opponentPawnStarts) == 0)
        return 0ULL;

    // must end up north or south (if black) of the other pawns
    if (piece == WP) { opponentPawnStarts <<= 8; } else { opponentPawnStarts >>= 8; }

    auto allValidEnPassantTargetSquares = opponentPawnStarts;
    return getPseudoPawnAttacks(piece, fromSquare) & allValidEnPassantTargetSquares;
}

Bitboard Rules::getPseudoPawnPushes(const Piece& piece, const int fromSquare){
    if (piece == WP) { return whitePawnPushes[fromSquare]; }
    if (piece == BP) { return blackPawnPushes[fromSquare]; }
    return 0ULL;
}

Bitboard Rules::getPseudoPawnAttacks(const Piece& piece, const int fromSquare){
    if (piece == WP) { return whitePawnAttacks[fromSquare]; }
    if (piece == BP) { return blackPawnAttacks[fromSquare]; }
    return 0ULL;
}

Bitboard Rules::getPseudoAttacks(const Piece& piece, const int fromSquare){
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