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

    if (status == BoardStatus::NORMAL) {
        bitboards.undoMove(move);
        swapTurns();
        return true;
    }

    // if the move results in, or leaves our king in check we can't do it.
    if (lastTurnInCheck(move)) {
        undoMove(move);
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }
    // if we give check - see if it's checkmate
    if (turnToMoveInCheck(move)) {
        // update the move result
        move.resultBits |= CHECK;
        move.resultBits &= ~PUSH;

        // should also check if its mate now
        if (isNowCheckMate()) {
            checkMateFlag = true;
            move.resultBits |= CHECK_MATE;
        }
    }

    undoMove(move);
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
    if (moveHistory.size() > 0 && moveHistory.top().resultBits & MoveResult::CHECK_MATE) {
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

bool BoardManager::turnToMoveInCheck(Move& move){
    const auto kingToMove = getCurrentTurn() == WHITE ? WK : BK;
    const auto opposingPawn = (kingToMove == WK) ? BP : WP;

    // will send out attackers of this turn colour, to all squares they could possibly attack
    const auto fakeAttackers = (kingToMove == WK)
                                   ? std::array{WN, WQ, WR, WB, WK}
                                   : std::array{BN, BQ, BR, BB, BK};

    // need this, to then refer to to see if the attacks overlap with real pieces
    const auto realAttackers = (kingToMove == BK)
                                   ? std::array{WN, WQ, WR, WB, WK}
                                   : std::array{BN, BQ, BR, BB, BK};

    const auto kingToMoveLocation = bitboards[kingToMove];

    bool checkFurther = false;
    std::array<Piece, 6> piecesThatGiveCheck; // only need six pieces
    std::ranges::fill(piecesThatGiveCheck, PIECE_N);

    Bitboard possibleAttacks = 0ULL;

    // the pawns are the only directional pieces, so have to check the colours the "right" way around
    possibleAttacks |= magicBitBoards.findAttacksForPiece(opposingPawn, bitboards);
    if (possibleAttacks & kingToMoveLocation) {
        move.resultBits |= CHECK;
        move.resultBits &= ~PUSH; // undo the push bit
        return true;
    }

    // see if we could in theory get to any of the occupied squares
    int pieceIndex = 0;
    for (const auto& pieceName: fakeAttackers) {
        const auto realAttacker = realAttackers[pieceIndex];
        const auto psuedoAttacks = magicBitBoards.rules.getPseudoAttacks(
            pieceName, std::countr_zero(kingToMoveLocation));
        if (psuedoAttacks & bitboards.getOccupancy(realAttacker)) {
            checkFurther = true;
            piecesThatGiveCheck[pieceIndex] = realAttacker; // cache which pieces so we only have to verify those
        }
        pieceIndex++;
    }

    if (checkFurther) {
        for (const auto& pieceName: piecesThatGiveCheck) {
            if (pieceName == PIECE_N) continue; // this piece can't possibly attack us

            // get it's real attacks
            possibleAttacks |= magicBitBoards.findAttacksForPiece(pieceName, bitboards);
            if (possibleAttacks & kingToMoveLocation) {
                move.resultBits |= CHECK;
                move.resultBits &= ~PUSH; // undo the push bit
                return true;
            }
        }
    }

    return false;
}

bool BoardManager::turnToMoveInCheck(){
    const auto kingToMove = getCurrentTurn() == WHITE ? WK : BK;
    const auto kingToMoveLocation = bitboards[kingToMove];

    const auto colourToSearch = getCurrentTurn() == WHITE ? BLACK : WHITE;

    const auto possibleAttacks = magicBitBoards.findAttacksForColour(colourToSearch, bitboards);
    if (possibleAttacks & kingToMoveLocation) { return true; }

    return false;
}


bool BoardManager::isNowCheckMate(){ return !hasLegalMoveToEscapeCheck(); }

bool BoardManager::hasLegalMoveToEscapeCheck(){
    if (currentTurn == WHITE) {
        // king first - most likely to escape check I guess?
        for (const auto& pieceName: {WN, WK, WQ, WR, WB, WP}) { if (canPieceEscapeCheck(pieceName)) { return true; } }
    } else {
        for (const auto& pieceName: {BN, BK, BQ, BR, BB, BP}) { if (canPieceEscapeCheck(pieceName)) { return true; } }
    }
    return false;
}

bool BoardManager::canPieceEscapeCheck(const Piece& pieceName){
    auto startingBoard = bitboards[pieceName];
    while (startingBoard) {
        // count trailing zeros to find the index of the first set bit
        const int startSquare = std::countr_zero(startingBoard);
        startingBoard &= startingBoard - 1;
        const auto possibleMoves = magicBitBoards.getMoves(startSquare, pieceName, bitboards);
        if (hasValidMoveFromSquare(pieceName, startSquare, possibleMoves)) { return true; }
    }
    return false;
}

bool BoardManager::hasValidMoveFromSquare(const Piece pieceName, const int startSquare,
                                          Bitboard destinationSquares){
    while (destinationSquares) {
        // count trailing zeros to find the index of the first set bit
        const int destinationSquare = std::countr_zero(destinationSquares);
        destinationSquares &= ~(1ULL << destinationSquare);
        auto move = Move(pieceName, startSquare, destinationSquare);

        if (isValidEscapeMove(move)) { return true; }
    }

    return false;
}


bool BoardManager::isValidEscapeMove(Move& move){
    if (!validateMove(move)) { return false; } // move not even pseudolegal
    // now we need to check the board state for check mates etc
    makeMove(move);

    // if the move results in, or leaves our king in check we can't do it.
    if (lastTurnInCheck(move)) {
        undoMove(move);
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    undoMove(move);
    return true;
}

bool BoardManager::lastTurnInCheck(const Move& move){
    const Piece lastTurnPiece = currentTurn == BLACK ? WK : BK;
    const Bitboard& kingLocation = bitboards[lastTurnPiece];

    if (currentTurn == WHITE) {
        for (const auto& pieceName: {WP, WQ, WK, WR, WB, WN}) {
            auto startingBoard = bitboards[pieceName];
            while (startingBoard) {
                // count trailing zeros to find the index of the first set bit
                const int startSquare = std::countr_zero(startingBoard);
                startingBoard &= startingBoard - 1;
                Bitboard prelimAttacks = 0ULL;

                // see if they can even attack in theory
                magicBitBoards.rules.getPseudoAttacks(pieceName, startSquare, prelimAttacks);
                if (!(kingLocation & prelimAttacks))
                    continue;
                Bitboard possibleMoves = 0ULL;
                magicBitBoards.getMoves(startSquare, pieceName, bitboards, possibleMoves);
                if (possibleMoves & kingLocation) { return true; }
            }
        }
    } else {
        for (const auto& pieceName: {BP, BQ, BK, BR, BB, BN}) {
            auto startingBoard = bitboards[pieceName];
            while (startingBoard) {
                // count trailing zeros to find the index of the first set bit
                const int startSquare = std::countr_zero(startingBoard);
                startingBoard &= startingBoard - 1;
                Bitboard prelimAttacks = 0ULL;

                // see if they can even attack in theory
                magicBitBoards.rules.getPseudoAttacks(pieceName, startSquare, prelimAttacks);
                if (!(kingLocation & prelimAttacks))
                    continue;
                Bitboard possibleMoves = 0ULL;
                magicBitBoards.getMoves(startSquare, pieceName, bitboards, possibleMoves);
                if (possibleMoves & kingLocation) { return true; }
            }
        }
    }

    return false;
}

std::string BoardManager::getFullFen(){
    const auto baseFen = bitboards.getFenPositionOnly();
    const auto enPassantSquareString = (boardStateHistory.top().enPassantSquare == -1)
                                           ? "-"
                                           : Fen::squareToFen(boardStateHistory.top().enPassantSquare);
    auto resultFen = baseFen + (currentTurn == WHITE ? " w" : " b") + " KQkq " + enPassantSquareString + " 0 1";
    return resultFen;
}