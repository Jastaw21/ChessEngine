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
    PUSH = 1 << 0, // 0000 0001 - 1
    CAPTURE = 1 << 1, // 0000 0010 - 2
    EN_PASSANT = 1 << 2, // 0000 0100 - 4
    CASTLING = 1 << 3, // 0000 1000 - 8
    CHECK = 1 << 4, // 0001 0000 - 16

    // failures
    ILLEGAL_MOVE = 1 << 5, // 0010 0000 - 32
    KING_IN_CHECK = 1 << 6, // 0100 0000 - 64
    CHECK_MATE = 1 << 7, // 1000 0000 - 128
};


struct Move {
    Move() = default;

    Move(const Piece& piece, const int fromSquare, const int toSquare) : piece(piece),
                                                                         rankFrom(squareToRank(fromSquare)),
                                                                         fileFrom(squareToFile(fromSquare)),
                                                                         rankTo(squareToRank(toSquare)),
                                                                         fileTo(squareToFile(toSquare)){};

    Move(const Piece& piece, const int rankFrom, const int fileFrom, const int rankTo, const int fileTo) : piece(piece),
        rankFrom(rankFrom), fileFrom(fileFrom), rankTo(rankTo), fileTo(fileTo){};
    Piece piece = PIECE_N;
    int rankFrom = 0;
    int fileFrom = 0;
    int rankTo = 0;
    int fileTo = 0;

    int resultBits = 0ULL;
    Piece capturedPiece = PIECE_N;

    std::string toUCI() const;
};

Move createMove(const Piece& piece, const std::string& moveUCI);

class BoardManager {
public:

    explicit BoardManager();
    explicit BoardManager(const Colours colour) : currentTurn(colour){}

    BitBoards *getBitboards(){ return &bitboards; }
    std::stack<Move> &getMoveHistory(){ return moveHistory; }
    MagicBitBoards *getMagicBitBoards(){ return &magicBitBoards; }

    bool opponentKingInCheck(Move& move);
    bool checkMove(Move& move);
    bool tryMove(Move& move);
    bool tryMove(const std::string& moveUCI);
    bool forceMove(Move& move);
    void undoMove(const Move& move);
    void undoMove();
    bool isGameOver();

    void setFen(const std::string& fen);

    Colours getCurrentTurn() const{ return currentTurn; }
    void setCurrentTurn(const Colours current_turn){ currentTurn = current_turn; }

    bool isNowCheckMate();
    std::vector<int> getStartingSquaresOfPiece(const Piece& piece);
    std::string getFullFen();

private:

    bool handleCapture(Move& move) const;
    bool checkAndHandleEP(Move& move);
    bool prelimCheckMove(Move& move);

    bool friendlyKingInCheck(const Move& move);

    // do the move
    void makeMove(Move& move);

    bool hasLegalMoveToEscapeCheck();
    bool canPieceEscapeCheck(const Piece& pieceName);
    bool hasValidMoveFromSquare(Piece pieceName, int startSquare, Bitboard& destinationSquares);
    bool isValidEscapeMove(Move& move);
    bool hasValidMoveFromSquare(Piece pieceName, int startSquare,
                                const std::bitset<64>& destinationSquares);

    // data
    BitBoards bitboards{};
    std::stack<Move> moveHistory;
    Colours currentTurn = WHITE;

    Rules rules;
    MagicBitBoards magicBitBoards;

    bool isCheck = false;
};


#endif //BOARDMANAGER_H