//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include <stack>

#include "BitBoards.h"
#include "Rules.h"
#include "Engine/Piece.h"

class BitBoards;

enum MoveResultBits {
    // successes
    bPUSH = 1 << 0, // 0000 0001
    bCAPTURE = 1 << 1, // 0000 0010
    bEN_PASSANT = 1 << 2, // 0000 0100
    bCASTLING = 1 << 3, // 0000 1000
    bCHECK = 1 << 4, // 0001 0000

    // failures
    bILLEGAL_MOVE = 1 << 5,
    bSQUARE_OCCUPIED = 1 << 6,
    bMOVE_NOT_LEGAL_FOR_PIECE = 1 << 7,
    bMOVE_OUT_OF_BOUNDS = 1 << 8,
    bBLOCKING_PIECE = 1 << 9,
    bKING_IN_CHECK = 1 << 10,
    bDISCOVERED_CHECK = 1 << 11
};


enum MoveResult {
    // successes
    PUSH, //0
    CAPTURE, //1

    // failures
    ILLEGAL_MOVE, //2
    SQUARE_OCCUPIED, //3
    MOVE_NOT_LEGAL_FOR_PIECE, //4
    MOVE_OUT_OF_BOUNDS, //5
    BLOCKING_PIECE, //6
    KING_IN_CHECK, //7
    EN_PASSANT, //8
    DISCOVERED_CHECK, //9
    CASTLING, //10
    CHECK, //11
};

struct Move {
    Piece piece;
    int rankFrom;
    int fileFrom;
    int rankTo;
    int fileTo;

    bool checkedOpponent = false;

    MoveResult result;
    int resultBits;
    Piece capturedPiece = PIECE_N;

    std::string toUCI() const;
};

Move createMove(const Piece& piece, const std::string& moveUCI);

class BoardManager {
public:

    explicit BoardManager();
    explicit BoardManager(Colours colour) : currentTurn(colour){};

    BitBoards *getBitboards(){ return &bitboards; }
    std::stack<Move> &getMoveHistory(){ return moveHistory; }

    bool opponentKingInCheck(Move& move);
    bool checkMove(Move& move);
    bool tryMove(Move& move);
    void undoMove(const Move& move);
    void undoMove();

    Colours getCurrentTurn() const{ return currentTurn; }
    void setCurrentTurn(const Colours current_turn){ currentTurn = current_turn; }

private:

    bool prelimCheckMove(Move& move);

    bool moveIsEnPassant(Move& move);
    bool friendlyKingInCheck(const Move& move);
    std::vector<int> getStartingSquaresOfPiece(const Piece& piece);

    // do the move
    void makeMove(Move& move);

    // data
    BitBoards bitboards{};
    std::stack<Move> moveHistory;
    Colours currentTurn = WHITE;

    Rules rules;
};


#endif //BOARDMANAGER_H