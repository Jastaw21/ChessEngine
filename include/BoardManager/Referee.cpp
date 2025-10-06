//
// Created by jacks on 06/10/2025.
//

#include "Referee.h"
#include "BitBoards.h"
#include "BoardManager/Move.h"
#include "BoardManager/BitBoards.h"

bool Referee::moveIsLegal(Move& move, BitBoards& boardState){}

bool Referee::validateMove(Move& move, BitBoards& boardState){
    const int fromSquare = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const int toSquare = rankAndFileToSquare(move.rankTo, move.fileTo);

    const Bitboard toSquareBitboard = 1ULL << toSquare;
    const auto friendlyColour = move.piece <= 5 ? WHITE : BLACK;
    const auto friendlyPieces = boardState.getOccupancy(friendlyColour);
    const auto enemyPieces = boardState.getOccupancy() & ~friendlyPieces;
}