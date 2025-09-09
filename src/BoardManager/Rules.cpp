//
// Created by jacks on 06/07/2025.
//
#include "BoardManager/Rules.h"

Rules::Rules(){
    for (int square = 0; square < 64; square++) {
        Bitboard rankResult = 0ULL;
        buildRankAttacks(square, rankResult);
        rankAttacks2[square] = rankResult;

        Bitboard fileResult = 0ULL;
        buildFileAttacks(square, fileResult);
        fileAttacks2[square] = fileResult;

        Bitboard diagResult = 0ULL;
        buildDiagonalAttacks(square, diagResult);
        diagAttacks2[square] = diagResult;

        Bitboard knightResult = 0ULL;
        buildKnightAttacks(square, knightResult);
        knightAttacks2[square] = knightResult;

        Bitboard whitePawnPushResult = 0ULL;
        buildWhitePawnPushes(square, whitePawnPushResult);
        whitePawnPushes2[square] = whitePawnPushResult;

        Bitboard blackPawnPushResult = 0ULL;
        buildBlackPawnPushes(square, blackPawnPushResult);
        blackPawnPushes2[square] = blackPawnPushResult;

        Bitboard whitePawnAttackResult = 0ULL;
        buildWhitePawnAttacks(square, whitePawnAttackResult);
        whitePawnAttacks2[square] = whitePawnAttackResult;

        Bitboard blackPawnAttackResult = 0ULL;
        buildBlackPawnAttacks(square, blackPawnAttackResult);
        blackPawnAttacks2[square] = blackPawnAttackResult;

        Bitboard kingPushResult = 0ULL;
        buildKingMoves(square, kingPushResult);
        kingMoves2[square] = kingPushResult;
    }
}

Bitboard Rules::getPseudoCastlingMoves(const Piece& piece, const int fromSquare, const BitBoards& boards){
    // only kings
    if (piece != WK && piece != BK)
        return 0ULL;

    int fileFrom, rankFrom;
    squareToRankAndFile(fromSquare, rankFrom, fileFrom);

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
        if (boards.getOccupancy() & 1ULL << rankAndFileToSquare(rankFrom, file))
            continue; // can't go to an occupied square

        Bitboard passingSquares = 0ULL;
        const int deltaFile = file - fileFrom;
        for (int intermediateFile = fileFrom + MathUtility::sign(deltaFile); intermediateFile != file;
             intermediateFile += MathUtility::sign(deltaFile)) {
            passingSquares |= 1ULL << rankAndFileToSquare(rankFrom, intermediateFile);
        }

        if (passingSquares & boards.getOccupancy())
            continue; // can't pass through an occupied square

        result |= 1ULL << rankAndFileToSquare(rankFrom, file);
    }

    return result;
}