//
// Created by jacks on 15/10/2025.
//

#ifndef CHESS_MOVEGENERATOR_H
#define CHESS_MOVEGENERATOR_H
#include <vector>

#include "BoardManager/BoardManager.h"
#include "BoardManager/Move.h"

using Moves = std::vector<Move>;

class MoveGenerator {
public:

    static Moves getMoves(BoardManager& manager);

private:

    static void processPawnPromotion(std::vector<Move>& moves, Move& move, BoardManager& manager);
    static void generateValidMovesFromPosition(Piece piece, int startSquare, std::vector<Move>& moves,
                                               BoardManager& manager);
    static void generateMovesForPiece(const Piece& piece, std::vector<Move>& moveList, BoardManager& manager);
};


#endif //CHESS_MOVEGENERATOR_H