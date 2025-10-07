//
// Created by jacks on 06/10/2025.
//

#ifndef CHESS_REFEREE_H
#define CHESS_REFEREE_H

#include "MagicBitboards/MagicBitBoards.h"

struct Move;
class BitBoards;

enum BoardStatus {
    NORMAL,
    WHITE_CHECK,
    BLACK_CHECK,
    WHITE_CHECKMATE,
    BLACK_CHECKMATE,
};

class Referee {
public:

    static bool moveIsLegal(Move& move, BitBoards& boardState, MagicBitBoards& mbb, const int enPassantSquare = -1);
    static int checkBoardStatus(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                Colours colourToMove);

private:

    static bool boardIsInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                               Colours colourToMove);
    static bool handleCapture(Move& move, const BitBoards& bitboards);
    static void handleEnPassant(Move& move);
    static bool validateMove(Move& move, const BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                             const int enPassantSquare);
    static bool validatePromotion(const Move& move);
    static bool lastTurnInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                Colours currentTurn);
    static bool currentTurnInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                   Colours currentTurn);

    static bool hasLegalMoveToEscapeCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                          Colours currentTurn);
    static void makeMove(const Move& move);
    static bool isValidEscapeMove(Move& move, BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                  Colours currentTurn);
    static bool hasValidMoveFromSquare(Piece piece, int startSquare, Bitboard destinationSquares, BitBoards& bitboards,
                                       MagicBitBoards& magicBitBoards, Colours currentTurn);
    static bool canPieceEscapeCheck(const Piece& pieceName, BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                    Colours currentTurn);
};
#endif //CHESS_REFEREE_H