//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include "BitBoards.h"
#include "Engine/Piece.h"

class BitBoards;

enum MoveResult {
    // successes
    MOVE_TO_EMPTY_SQUARE,
    PIECE_CAPTURE,

    // failures
    ILLEGAL_MOVE,
    SQUARE_OCCUPIED,
    MOVE_NOT_LEGAL_FOR_PIECE,
    MOVE_OUT_OF_BOUNDS,
    BLOCKING_PIECE,
    KING_IN_CHECK,
};

struct Move {
    Piece piece;
    int rankFrom;
    int fileFrom;
    int rankTo;
    int fileTo;

    MoveResult result;
    Piece capturedPiece = Piece::PIECE_N;

    std::string toUCI() const;
};

Move createMove(const Piece& piece, const std::string& moveUCI);

class BoardManager {
public:

    explicit BoardManager();

    BitBoards *getBitboards(){ return &bitboards; }

    bool tryMove(Move& move);
    bool captureIsLegal(Move& move);
    bool checkMove(Move& move);
    void undoMove(const Move& move);

private:

    static bool moveIsLegalForPiece(const Move& move);
    static bool moveInBounds(const Move& move);
    bool pieceInWay(const Move& move) const;
    [[nodiscard]] bool moveDestinationIsEmpty(const Move& move) const;
    bool moveDestOccupiedByColour(const std::string& testColour, const Move& move) const;

    bool kingInCheck(const Piece& piece, const Move& move);

    void makeMove(const Move& move);

    BitBoards bitboards{};
};


#endif //BOARDMANAGER_H