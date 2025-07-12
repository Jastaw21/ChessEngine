//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include <stack>

#include "BitBoards.h"
#include "Rules.h"
#include "Engine/Piece.h"
#include "MagicBitboards/MagicBitBoards.h"


class BitBoards;
class MagicBitBoards;

enum MoveResult {
    // successes
    PUSH = 1 << 0, // 0000 0001
    CAPTURE = 1 << 1, // 0000 0010
    EN_PASSANT = 1 << 2, // 0000 0100
    CASTLING = 1 << 3, // 0000 1000
    CHECK = 1 << 4, // 0001 0000

    // failures
    ILLEGAL_MOVE = 1 << 5,
    SQUARE_OCCUPIED = 1 << 6,
    MOVE_NOT_LEGAL_FOR_PIECE = 1 << 7,
    MOVE_OUT_OF_BOUNDS = 1 << 8,
    BLOCKING_PIECE = 1 << 9,
    KING_IN_CHECK = 1 << 10,
    DISCOVERED_CHECK = 1 << 11
};


struct Move {
    Piece piece;
    int rankFrom;
    int fileFrom;
    int rankTo;
    int fileTo;

    bool checkedOpponent = false;

    int resultBits;
    Piece capturedPiece = PIECE_N;

    std::string toUCI() const;
};

Move createMove(const Piece& piece, const std::string& moveUCI);

class BoardManager {
public:

    explicit BoardManager();
    explicit BoardManager(Colours colour) : currentTurn(colour){}

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

    Bitboard getSliderMoves(const Piece& movePiece, int fromSquare, const Bitboard& board);
    bool handleCapture(Move& move);
    void handleEP(Move& move);
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

    MagicBitBoards magicBitBoards;
};


#endif //BOARDMANAGER_H