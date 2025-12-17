//
// Created by jacks on 07/09/2025.
//

#ifndef CHESS_EVALUATIONVALUES_H
#define CHESS_EVALUATIONVALUES_H

#include <array>

#include "Piece.h"

// clang-format off

constexpr std::array pawnScores = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50,50,50,50,50,50,50,50,
            10,10,20,30,30,20,10,10,
            5, 5, 10,25,25,10, 5, 5,
            0, 0, 0, 20,20, 0, 0, 0,
            5,-5,-10,0, 0, -10,-5,5,
            5,10,10,-20,-20,10,10,5,
            0, 0, 0, 0, 0, 0, 0, 0
        };

constexpr std::array knightScores = {
        -50,-40,-30,-30,-30,-30,-40,-50,
         -40,-20,  0,  0,  0,  0,-20,-40,
         -30,  0, 10, 15, 15, 10,  0,-30,
         -30,  5, 15, 20, 20, 15,  5,-30,
         -30,  0, 15, 20, 20, 15,  0,-30,
         -30,  5, 10, 15, 15, 10,  5,-30,
         -40,-20,  0,  5,  5,  0,-20,-40,
         -50,-40,-30,-30,-30,-30,-40,-50,
};

constexpr std::array bishopScores = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
};

constexpr std::array queenScores = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,   0,  5,  5,  5,  5,  0, -5,
        0,    0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
};

constexpr std::array rookScores = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
};


constexpr std::array zeroScores = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
// clang-format on


inline const float MATE_SCORE = 10000;

constexpr std::array<float, PIECE_N> pieceScoresArray = {
            1000, 310, 320, 500, 900, 1000,
            100, 310, 320, 5, 900, 1000
        };

inline const std::array<int, 64>& getPieceScores(PieceType pieceType){
    switch (pieceType) {
        case PAWN:
            return pawnScores;
        case KNIGHT:
            return knightScores;
        case BISHOP:
            return bishopScores;
        case QUEEN:
            return queenScores;
        case ROOK:
            return rookScores;
        default:
            return zeroScores;
    }
}

inline const std::array<int, 64>& getPieceScores(Piece piece){
    switch (piece) {
        case WP:
        case BP:
            return pawnScores;
        case WN:
        case BN:
            return knightScores;
        case WB:
        case BB:
            return bishopScores;
        case WQ:
        case BQ:
            return queenScores;
        case WR:
        case BR:
            return rookScores;
        default:
            return zeroScores;
    }
}


inline int flipBoard(const int square){ return 63 - square; }

#endif //CHESS_EVALUATIONVALUES_H