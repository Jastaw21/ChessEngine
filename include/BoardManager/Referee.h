//
// Created by jacks on 06/10/2025.
//

#ifndef CHESS_REFEREE_H
#define CHESS_REFEREE_H

#include "MagicBitboards/MagicBitBoards.h"

struct Move;
class BitBoards;

enum BoardStatus {
    NORMAL = 1 << 0,
    WHITE_CHECK = 1 << 1,
    BLACK_CHECK = 1 << 2,
    WHITE_CHECKMATE = 1 << 3,
    BLACK_CHECKMATE = 1 << 4,
};

class Referee {
public:

    static bool moveIsLegal(Move& move, BitBoards& boardState, MagicBitBoards& mbb, const int enPassantSquare = -1);
    static int checkBoardStatus(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                Colours colourToMove);
    bool isStalemate();
    static bool hasAnyLegalMoves(BitBoards& bitBoards, MagicBitBoards& magicBitBoards,
                                 Colours colourToMove);

private:

    static bool boardIsInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                               Colours colourToMove);
    static bool handleCapture(Move& move, const BitBoards& bitboards);
    static void handleEnPassant(Move& move);
    static bool validateMove(Move& move, const BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                             const int enPassantSquare);
    static bool validatePromotion(const Move& move);


    static bool hasLegalMoveToEscapeCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                          Colours currentTurn);

    static bool isValidEscapeMove(Move& move, BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                  Colours currentTurn);
    static bool hasValidMoveFromSquare(Piece piece, int startSquare, Bitboard destinationSquares, BitBoards& bitboards,
                                       MagicBitBoards& magicBitBoards, Colours currentTurn);
    static bool canPieceEscapeCheck(const Piece& pieceName, BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                    Colours currentTurn);

    static bool isKingInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                              Colours currentTurn);
};
#endif //CHESS_REFEREE_H