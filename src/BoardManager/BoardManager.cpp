//ReSharper disable CppTooWideScopeInitStatement


#include "BoardManager/BoardManager.h"

#include <iostream>
#include <sstream>

#include "BoardManager/BitBoards.h"
#include "BoardManager/Rules.h"
#include "BoardManager/Referee.h"
#include "Utility/Fen.h"


/**
* Checks if a move is legal - in all elements other than if it leaves the moving king in check
* @param move - the move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success
**/
bool BoardManager::validateMove(Move& move){
    move.resultBits = 0; // reset the move result tracker
    return Referee::moveIsLegal(
        move,
        bitboards,
        magicBitBoards,
        boardStateHistory.top().enPassantSquare
    );
}


/**
* Checks if a move is completely legal - including checkmate/check tests
* @param move - the move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success
**/
bool BoardManager::checkMove(Move& move){
    if (boardStateHistory.empty()) {
        boardStateHistory.push(
            BoardState{.enPassantSquare = -1, .castlingRights = ""}
        );
    }
    const bool isPseudoLegal =
            Referee::moveIsLegal(
                move,
                bitboards,
                magicBitBoards,
                boardStateHistory.top().enPassantSquare
            );
    if (!isPseudoLegal) { return false; } // move not even pseudolegal

    // now we need to check the board state for check mates etc
    bitboards.applyMove(move);
    swapTurns();
    auto status = Referee::checkBoardStatus(
        bitboards,
        magicBitBoards,
        currentTurn
    );

    // no checks
    if (status == BoardStatus::NORMAL) {
        bitboards.undoMove(move);
        swapTurns();
        return true;
    }

    bool blackInCheckOrMate = status & (BLACK_CHECK | BLACK_CHECKMATE);
    bool whiteInCheckOrMate = status & (WHITE_CHECK | WHITE_CHECKMATE);

    // leaves the previous turn to move in check
    if (blackInCheckOrMate && currentTurn == WHITE || whiteInCheckOrMate && currentTurn == BLACK) {
        bitboards.undoMove(move);
        swapTurns();
        move.resultBits = ILLEGAL_MOVE;
        return false;
    }

    // we gave checkmate on the last move
    if (status & BoardStatus::BLACK_CHECKMATE || status & BoardStatus::WHITE_CHECKMATE) {
        move.resultBits |= CHECK;
        move.resultBits &= ~PUSH;
        move.resultBits |= CHECK_MATE;
        checkMateFlag = true;
        bitboards.undoMove(move);
        swapTurns();
        return true;
    }

    // we gave check on the last move
    if (status & BoardStatus::BLACK_CHECK || status & BoardStatus::WHITE_CHECK) {
        move.resultBits |= CHECK;
        move.resultBits &= ~PUSH; // undo the push bit
        bitboards.undoMove(move);
        swapTurns();
        return true;
    }

    bitboards.undoMove(move);
    swapTurns();
    return true;
}

/**
* Checks and applies a move if legal. Updates internal board state
* @param move - the move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success
**/

bool BoardManager::tryMove(Move& move){
    if (!checkMove(move)) { return false; }
    makeMove(move);
    return true;
}

/**
* Constructs a move object from a UCI move string. Then checks and applies a move if legal. Updates internal board state
* @param moveUCI - the move to be checked in UCI string format. This will be edited to reflect the outcome of the move
* @return boolean success
**/
bool BoardManager::tryMove(const std::string& moveUCI){
    const auto fromSquare = Fen::FenToSquare(moveUCI);
    const auto optionalPiece = bitboards.getPiece(fromSquare);
    if (!optionalPiece.has_value()) {
        std::cout << "Error: No piece at location " << fromSquare << std::endl;
        return false;
    }
    const auto piece = optionalPiece.value();
    auto move = createMove(piece, moveUCI);
    return tryMove(move);
}

/**
* Forces a move to happen without legality check. Intended for use when moves have been checked during generation
* @param move - The move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success - always true
**/
bool BoardManager::forceMove(Move& move){
    makeMove(move);
    return true;
}

/**
* Applies a move, updating the relevant elements of the board state
* @param move - the move to be applied. This should have been checked before.
**/
void BoardManager::makeMove(Move& move){
    BoardState newBoardState;

    // handle the EP square
    int enPassantSquareState;
    if ((move.piece == WP || move.piece == BP)
        && move.fileTo == move.fileFrom
        && abs(move.rankTo - move.rankFrom) == 2
    ) {
        // a pawn has moved 2 ranks - and exposed an en passant square - update our state tracker
        const auto targetRank = move.piece == WP ? move.rankTo - 1 : move.rankTo + 1;
        enPassantSquareState = rankAndFileToSquare(targetRank, move.fileFrom);
    } else {
        // any other move means the en passant square is now invalid
        enPassantSquareState = -1;
    }

    bitboards.applyMove(move);
    swapTurns();

    zobristHash_.addMove(move);

    repetitionTable_New_.addMove(zobristHash_.getHash());
    if (repetitionTable_New_.checkForRepetition()) { repetitionFlag = true; }

    moveHistory.emplace(move);
    boardStateHistory.emplace(BoardState{.enPassantSquare = enPassantSquareState, .castlingRights = ""});
}


void BoardManager::swapTurns(){
    if (currentTurn == WHITE)
        currentTurn = BLACK;
    else
        currentTurn = WHITE;
}

void BoardManager::undoMove(const Move& move){
    bitboards.undoMove(move);

    moveHistory.pop();
    boardStateHistory.pop();

    swapTurns();

    repetitionTable_New_.popMove();
    zobristHash_.undoMove(move);
    repetitionFlag = false;
    checkMateFlag = false; // can never undo into a checkmate position
}

void BoardManager::undoMove(){
    if (moveHistory.empty())
        return;
    undoMove(moveHistory.top());
}

bool BoardManager::threefoldRepetition(){ return repetitionFlag; }

bool BoardManager::isGameOver(){
    if (moveHistory.size() >= 100) { return true; }
    if (checkMateFlag) { return true; }
    if (threefoldRepetition()) { return true; }

    return false;
}

int BoardManager::getGameResult(){
    int resultBits = 0;

    if (moveHistory.size() >= 100) {
        resultBits |= GameResult::DRAW;
        resultBits |= GameResult::MOVE_COUNT;
        return resultBits;
    }

    if (threefoldRepetition()) {
        resultBits |= GameResult::DRAW;
        resultBits |= GameResult::REPETITION;
        return resultBits;
    }

    const auto lastMoveColour = currentTurn == WHITE ? BLACK : WHITE;
    if (checkMateFlag) {
        resultBits |= GameResult::CHECKMATE;

        if (lastMoveColour == WHITE)
            resultBits |= WHITE_WINS;
        else
            resultBits |= BLACK_WINS;

        return resultBits;
    }
    return resultBits;
}

void BoardManager::setFullFen(const FenString& fen){
    std::istringstream fenStream(fen);

    std::string line;
    std::string fenPiecePlacement;
    std::string fenActiveColour;
    std::string fenCastling;
    std::string fenEnPassant;
    std::string fenHalfMoveClock;
    std::string fenFullMoveNumber;

    fenStream >> fenPiecePlacement >> fenActiveColour >> fenCastling >> fenEnPassant >> fenHalfMoveClock >>
            fenFullMoveNumber;

    const auto enPassantSquare = fenEnPassant == "-" ? -1 : Fen::FenToSquare(fenEnPassant);
    boardStateHistory.emplace(BoardState{.enPassantSquare = enPassantSquare});
    bitboards.setFenPositionOnly(fenPiecePlacement);
    setCurrentTurn(fenActiveColour == "w" ? WHITE : BLACK);
    zobristHash_.setFen(fen);
}

std::string BoardManager::getFullFen(){
    const auto baseFen = bitboards.getFenPositionOnly();
    const auto enPassantSquareString = (boardStateHistory.top().enPassantSquare == -1)
                                           ? "-"
                                           : Fen::squareToFen(boardStateHistory.top().enPassantSquare);
    auto resultFen = baseFen + (currentTurn == WHITE ? " w" : " b") + " KQkq " + enPassantSquareString + " 0 1";
    return resultFen;
}