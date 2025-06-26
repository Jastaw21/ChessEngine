//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include <stack>

#include "BitBoards.h"
#include "Engine/Piece.h"

class BitBoards;

enum MoveResult {
    // successes
    MOVE_TO_EMPTY_SQUARE,       //0
    PIECE_CAPTURE,              //1

    // failures
    ILLEGAL_MOVE,               //2
    SQUARE_OCCUPIED,            //3
    MOVE_NOT_LEGAL_FOR_PIECE,   //4
    MOVE_OUT_OF_BOUNDS,         //5
    BLOCKING_PIECE,             //6
    KING_IN_CHECK,              //7
    EN_PASSANT,                 //8
    DISCOVERED_CHECK,           //9
};

struct Move {
    Piece piece;
    int rankFrom;
    int fileFrom;
    int rankTo;
    int fileTo;

    MoveResult result;
    Piece capturedPiece = PIECE_N;

    std::string toUCI() const;
};

Move createMove(const Piece& piece, const std::string& moveUCI);

class BoardManager {
public:

    explicit BoardManager();
    static bool moveIsLegalForPiece(int squareFrom, int squareTo, const Piece& piece);

    BitBoards *getBitboards(){ return &bitboards; }
    std::stack<Move> &getMoveHistory(){ return moveHistory; }

    bool tryMove(Move& move);

    bool checkMove(Move& move);
    void undoMove(const Move& move);
    void undoMove();

    Colours getCurrentTurn() const{ return currentTurn; }

private:

    // internal move legality checkers
    static bool captureIsLegal(Move& move);
    static bool moveIsLegalForPiece(const Move& move);
    static bool moveInBounds(const Move& move);
    bool pieceInWay(const Move& move) const;
    [[nodiscard]] bool moveDestinationIsEmpty(const Move& move) const;
    bool moveDestOccupiedByColour(const Move& move);
    bool moveIsEnPassant(Move& move) const;
    bool kingInCheck(const Move& move);
    bool checkWouldBeUncovered(Move& move);

    // do the move
    void makeMove(Move& move);

    // data
    BitBoards bitboards{};
    std::stack<Move> moveHistory;
    Colours currentTurn = WHITE;
};


#endif //BOARDMANAGER_H