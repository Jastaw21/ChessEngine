//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include <stack>

#include "BitBoards.h"
#include "Rules.h"
#include "Engine/Piece.h"

#include "../Engine/ZobristHash.h"

#include "MagicBitboards/MagicBitBoards.h"
#include "Utility/Fen.h"
#include "Engine/RepetitionTable.h"


class BitBoards;

struct BoardState {
    int enPassantSquare = -1;
    std::string castlingRights = "";
};

enum MoveResult {
    // successes
    PUSH = 1 << 0, // 0000 0001 - 1
    CAPTURE = 1 << 1, // 0000 0010 - 2
    EN_PASSANT = 1 << 2, // 0000 0100 - 4
    CASTLING = 1 << 3, // 0000 1000 - 8
    CHECK = 1 << 4, // 0001 0000 - 16

    // failures
    ILLEGAL_MOVE = 1 << 5, // 0010 0000 - 32
    PROMOTION = 1 << 6, // 0100 0000 - 64
    CHECK_MATE = 1 << 7, // 1000 0000 - 128
};

enum GameResult {
    WHITE_WINS = 1 << 0,
    BLACK_WINS = 1 << 1,
    DRAW = 1 << 2,
    REPETITION = 1 << 3,
    CHECKMATE = 1 << 4,
    STALEMATE = 1 << 5,
    IN_PROGRESS = 1 << 6,
    MOVE_COUNT = 1 << 7
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
    Piece promotedPiece = PIECE_N;

    std::string toUCI() const;

    bool operator==(const Move& other) const{
        return
                piece == other.piece
                && rankFrom == other.rankFrom
                && fileFrom == other.fileFrom
                && rankTo == other.rankTo
                && fileTo == other.fileTo
                && capturedPiece == other.capturedPiece
                && promotedPiece == other.promotedPiece;
    }

    bool operator!=(const Move& other) const{ return !(*this == other); }

    bool isInverseOf(const Move& move) const{
        return piece == move.piece
               && rankFrom == move.rankTo
               && fileFrom == move.fileTo
               && rankTo == move.rankFrom
               && fileTo == move.fileFrom;
    }
};

Move createMove(const Piece& piece, const std::string& moveUCI);
Move createMove(const Piece& piece, const RankAndFile squareFrom, const RankAndFile squareTo,
                const Piece promotedPiece = PIECE_N);

class BoardManager {
public:

    explicit BoardManager();
    explicit BoardManager(const Colours colour) : currentTurn(colour){}

    BitBoards* getBitboards(){ return &bitboards; }
    std::stack<Move>& getMoveHistory(){ return moveHistory; }
    MagicBitBoards* getMagicBitBoards(){ return &magicBitBoards; }
    int getEnPassantSquare(){ return boardStateHistory.top().enPassantSquare; }
    ZobristHash* getZobristHash(){ return &zobristHash_; }

    bool tryMove(Move& move);
    bool tryMove(const std::string& moveUCI);
    bool checkMove(Move& move);
    bool forceMove(Move& move);
    void undoMove(const Move& move);
    void undoMove();

    bool turnToMoveInCheck(Move& move);
    bool turnToMoveInCheck();

    bool isGameOver();
    int getGameResult();

    bool threefoldRepetition();

    void resetGame(){
        setFullFen(Fen::FULL_STARTING_FEN);
        while (!moveHistory.empty()) { moveHistory.pop(); }
        repetitionTable_New_.clear();
        while (!boardStateHistory.empty()) { boardStateHistory.pop(); }
    }

    void resetGame(const FenString& fen){
        setFullFen(fen);
        while (!moveHistory.empty()) { moveHistory.pop(); }
        repetitionTable_New_.clear();
    };

    void setFullFen(const FenString& fen);
    std::string getFullFen();

    Colours getCurrentTurn() const{ return currentTurn; }
    void setCurrentTurn(const Colours current_turn){ currentTurn = current_turn; }

    bool isNowCheckMate();

private:

    bool handleCapture(Move& move) const;
    void handleEnPassant(Move& move);
    bool validateMove(Move& move);

    bool lastTurnInCheck(const Move& move);

    // do the move
    void makeMove(Move& move);
    void applyCastlingMove(Move& move);
    void undoCastling(const Move& move);

    bool hasLegalMoveToEscapeCheck();
    bool canPieceEscapeCheck(const Piece& pieceName);
    bool hasValidMoveFromSquare(Piece pieceName, int startSquare, Bitboard destinationSquares);
    bool isValidEscapeMove(Move& move);
    void swapTurns();

    // data
    BitBoards bitboards{};
    std::stack<Move> moveHistory;
    std::stack<BoardState> boardStateHistory;
    RepetitionTable repetitionTable_New_;
    bool repetitionFlag = false;
    Colours currentTurn = WHITE;
    ZobristHash zobristHash_;

    Rules rules;
    MagicBitBoards magicBitBoards;

    bool checkMateFlag = false;
};


#endif //BOARDMANAGER_H