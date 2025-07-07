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
    CASTLING,
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
    explicit BoardManager(Colours colour) : currentTurn(colour){};

    BitBoards *getBitboards(){ return &bitboards; }
    std::stack<Move> &getMoveHistory(){ return moveHistory; }

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