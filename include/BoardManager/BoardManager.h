//
// Created by jacks on 18/06/2025.
//

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include <stack>

#include "BitBoards.h"

#include "BoardManager/Move.h"

#include "Engine/Piece.h"
#include "Engine/RepetitionTable.h"
#include "Engine/ZobristHash.h"

#include "MagicBitboards/MagicBitBoards.h"

#include "Utility/Fen.h"


class BitBoards;

struct BoardState {
    int enPassantSquare = -1;
    std::string castlingRights = "";
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


class BoardManager {
public:

    BoardManager() = default;
    explicit BoardManager(const Colours colour) : currentTurn(colour){}

    BitBoards* getBitboards(){ return &bitboards; }
    std::stack<Move>& getMoveHistory(){ return moveHistory; }
    MagicBitBoards* getMagicBitBoards(){ return &magicBitBoards; }
    int getEnPassantSquare(){ return boardStateHistory.top().enPassantSquare; }
    ZobristHash* getZobristHash(){ return &zobristHash_; }
    Colours getCurrentTurn() const{ return currentTurn; }
    void setCurrentTurn(const Colours current_turn){ currentTurn = current_turn; }

    bool tryMove(Move& move);
    bool tryMove(const std::string& moveUCI);
    bool checkMove(Move& move);
    bool forceMove(Move& move);
    void undoMove(const Move& move);
    void undoMove();

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

private:

    bool validateMove(Move& move);

    // do the move
    void makeMove(Move& move);

    void swapTurns();

    // data
    BitBoards bitboards{};
    std::stack<Move> moveHistory;
    std::stack<BoardState> boardStateHistory;
    RepetitionTable repetitionTable_New_;
    bool repetitionFlag = false;
    Colours currentTurn = WHITE;
    ZobristHash zobristHash_;

    MagicBitBoards magicBitBoards;

    bool checkMateFlag = false;
};


#endif //BOARDMANAGER_H