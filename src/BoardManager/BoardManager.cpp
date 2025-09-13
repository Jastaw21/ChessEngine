//ReSharper disable CppTooWideScopeInitStatement

#include "BoardManager/BoardManager.h"

#include <bitset>
#include <iostream>
#include <sstream>
#include <vector>

#include "BoardManager/BitBoards.h"
#include "BoardManager/Rules.h"
#include "Utility/Fen.h"

std::string Move::toUCI() const{
    auto coreMoveString = std::string{static_cast<char>('a' + fileFrom - 1)} + std::to_string(rankFrom) + static_cast<
                              char>(
                              'a' + fileTo - 1) + std::to_string(rankTo);

    if (promotedPiece == PIECE_N) { return coreMoveString; }

    const auto promotionString = PIECE_TO_CHAR_MAP[promotedPiece];
    return coreMoveString + promotionString;
}

Move createMove(const Piece& piece, const std::string& moveUCI){
    const int fileFrom = moveUCI[0] - 'a' + 1;
    const int rankFrom = moveUCI[1] - '1' + 1;
    const int fileTo = moveUCI[2] - 'a' + 1;
    const int rankTo = moveUCI[3] - '1' + 1;
    auto returnMove = Move(piece, rankAndFileToSquare(rankFrom, fileFrom), rankAndFileToSquare(rankTo, fileTo));

    // check if there's a requested promotion
    if (moveUCI.size() == 5) {
        const auto promotionPiece = moveUCI[4];

        Piece promotionPieceEnum;
        switch (promotionPiece) {
            case 'Q':
                promotionPieceEnum = WQ;
                break;
            case 'q':
                promotionPieceEnum = BQ;
                break;
            case 'R':
                promotionPieceEnum = WR;
                break;
            case 'r':
                promotionPieceEnum = BR;
                break;
            case 'N':
                promotionPieceEnum = WN;
                break;
            case 'n':
                promotionPieceEnum = BN;
                break;
            case 'B':
                promotionPieceEnum = WB;
                break;
            case 'b':
                promotionPieceEnum = BB;
                break;
            default:
                std::cout << "Error: Invalid promotion piece " << promotionPiece << std::endl;
                return returnMove;
        }
        returnMove.promotedPiece = promotionPieceEnum;
    }

    return returnMove;
}

BoardManager::BoardManager() = default;

/**
* Checks if a move is legal - in all elements other than if it leaves the moving king in check
* @param Move - the move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success
**/
bool BoardManager::validateMove(Move& move){
    if (boardStateHistory.empty()) { boardStateHistory.push(BoardState{.enPassantSquare = -1, .castlingRights = ""}); }
    move.resultBits = 0; // reset the move result tracker
    const int fromSquare = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const int toSquare = rankAndFileToSquare(move.rankTo, move.fileTo);

    const Bitboard toSquareBitboard = 1ULL << toSquare;
    const auto friendlyPieces = bitboards.getOccupancy(pieceColours[move.piece]);
    const auto enemyPieces = bitboards.getOccupancy() & ~friendlyPieces;

    // just get all of the possible moves
    const auto allMoves = magicBitBoards.getMoves(fromSquare, move.piece, bitboards);

    // if the to square doesn't appear in all, the move is therefore not any form of legal
    if (!(allMoves & toSquareBitboard)) {
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    // move would be on top of a friendly piece
    if (toSquareBitboard & friendlyPieces) {
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    // we are attacking an enemy, check it's legal (i.e. non-king)
    if (toSquareBitboard & enemyPieces) {
        bool captureHandled = handleCapture(move);
        if (!captureHandled) {
            move.resultBits = 0;
            move.resultBits |= ILLEGAL_MOVE;
            return false;
        }

        // if it's not a promotion capture - escape now
        if (move.promotedPiece == PIECE_N) { return true; }
    }

    // ep
    if (move.piece == WP || move.piece == BP) {
        if (boardStateHistory.top().enPassantSquare == toSquare) {
            handleEnPassant(move);
            return true;
        }

        if (move.fileTo != move.fileFrom && (move.resultBits & CAPTURE) == 0) {
            // if not EP or capture (handled above), going diagonal must be an illegal move
            move.resultBits = 0; // reset the move result tracker
            move.resultBits |= ILLEGAL_MOVE;
            return false;
        }
    }

    // castling
    const auto castlingMoves = RulesCheck::getCastlingMoves(fromSquare, move.piece, &bitboards);
    if (toSquareBitboard & castlingMoves) {
        move.resultBits |= CASTLING;
        return true;
    }

    // promotion
    if (move.promotedPiece != PIECE_N) {
        if (pieceColours[move.piece] != pieceColours[move.promotedPiece]) {
            move.resultBits |= ILLEGAL_MOVE;
            return false;
        }

        if (move.promotedPiece != WQ && move.promotedPiece != BQ && move.promotedPiece != WR &&
            move.promotedPiece != BR && move.promotedPiece != WN && move.promotedPiece != BN && move.promotedPiece != WB
            && move.promotedPiece != BB) {
            move.resultBits |= ILLEGAL_MOVE;
            return false;
        }
        move.resultBits |= PROMOTION;
    }

    // push
    move.resultBits |= PUSH;
    return true;
}


/**
* Checks if a move is completely legal - including checkmate/check tests
* @param Move - the move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success
**/
bool BoardManager::checkMove(Move& move){
    if (!validateMove(move)) { return false; } // move not even pseudolegal

    // now we need to check the board state for check mates etc
    makeMove(move);

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
* @param Move - the move to be checked. This will be edited to reflect the outcome of the move
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
* @param Move - the move to be checked. This will be edited to reflect the outcome of the move
* @return boolean success
**/
bool BoardManager::forceMove(Move& move){
    makeMove(move);
    return true;
}

/**
* Applies a move, updating the relevant elements of the board state
* @param Move - the move to be applied. This should have been checked before.
**/
void BoardManager::makeMove(Move& move){
    int enPassantSquareState;
    BoardState newBoardState;

    // handle the EP square
    if ((move.piece == WP || move.piece == BP)
        && move.fileTo == move.fileFrom
        && abs(move.rankTo - move.rankFrom) == 2
    ) {
        // a pawn has moved 2 ranks - and exposed an en passant square - update our state tracker
        auto targetRank = move.piece == WP ? move.rankTo - 1 : move.rankTo + 1;
        enPassantSquareState = rankAndFileToSquare(targetRank, move.fileFrom);
    } else {
        // any other move means the en passant square is now invalid
        enPassantSquareState = -1;
    }

    // always set the "from" square of the moving piece to zero
    bitboards.setZero(move.rankFrom, move.fileFrom);

    // castling needs special stuff doing
    if (move.resultBits & CASTLING) { applyCastlingMove(move); }

    // if it was a normal capture, set the piece to zero
    if (move.resultBits & CAPTURE && !(move.resultBits & EN_PASSANT))
        bitboards.setZero(move.rankTo, move.fileTo);

    // if it was an en_passant capture, set the correct square to zero
    if (move.resultBits & EN_PASSANT) {
        const auto rankOffset = move.piece == WP ? -1 : 1;
        bitboards.setZero(move.rankTo + rankOffset, move.fileTo);
    }

    // if it's not a promotion, set the to square of the moving piece to one
    if (!(move.resultBits & PROMOTION)) { bitboards.setOne(move.piece, move.rankTo, move.fileTo); }

    // otherwise, need to toggle on the bit for the piece it chose
    else { bitboards.setOne(move.promotedPiece, move.rankTo, move.fileTo); }

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

void BoardManager::applyCastlingMove(Move& move){
    const auto relevantRook = move.piece == WK ? WR : BR; // what is the rook we also need to move?

    int movedRookFileTo = 0;
    int movedRookFileFrom = 0;
    // queen side
    if (move.fileTo == 3) {
        movedRookFileTo = move.fileTo + 1; // needs to move one inside the king
        movedRookFileFrom = 1; //... from file 1
    }
    // king side
    else if (move.fileTo == 7) {
        movedRookFileTo = move.fileTo - 1; // needs to move one inside the king
        movedRookFileFrom = 8; //... from file 8
    }

    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    bitboards.setOne(relevantRook, move.rankTo, movedRookFileTo);
    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    bitboards.setZero(move.rankTo, movedRookFileFrom);
}


void BoardManager::undoCastling(const Move& move){
    bitboards.setZero(move.rankTo, move.fileTo);

    const auto relevantRook = move.piece == WK ? WR : BR;

    int movedRookFileTo;
    int movedRookFileFrom;
    // queen side
    if (move.fileTo == 3) {
        // set the new rook location
        movedRookFileTo = move.fileTo + 1;
        movedRookFileFrom = 1;
    }
    // king side
    else if (move.fileTo == 7) {
        movedRookFileTo = move.fileTo - 1;
        movedRookFileFrom = 8;
    }

    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    bitboards.setOne(relevantRook, move.rankFrom, movedRookFileFrom);
    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    bitboards.setZero(move.rankTo, movedRookFileTo);
}

void BoardManager::undoMove(const Move& move){
    // set the "from" bit back to one
    const auto squareFrom = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    bitboards[move.piece] |= 1ULL << squareFrom;

    // if it was a capture, restore that piece to one
    if (move.resultBits & CAPTURE && !(move.resultBits & EN_PASSANT)) {
        const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
        bitboards[move.capturedPiece] |= 1ULL << squareTo;
    }

    // if it was an en_passant capture, restore the correct square to one
    if (move.resultBits & EN_PASSANT) {
        const auto opponentPawn = move.piece == WP ? BP : WP;
        const auto rankOffset = move.piece == WP ? -1 : 1;
        bitboards.setOne(opponentPawn, move.rankTo + rankOffset, move.fileTo);
    }

    // if it was a castling move, restore the rooks to their original positions
    if (move.resultBits & CASTLING) { undoCastling(move); }

    // set the "to" bit back to zero for this piece
    const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
    if (move.resultBits & PROMOTION) {
        bitboards[move.promotedPiece] &= ~(1ULL << squareTo); // whatever we promoted to is gone
    }

    bitboards[move.piece] &= ~(1ULL << squareTo);

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
    const auto kingToMoveLocation = bitboards[kingToMove];

    const auto colourToSearch = getCurrentTurn() == WHITE ? BLACK : WHITE;

    const auto possibleAttacks = magicBitBoards.findAttacksForColour(colourToSearch, bitboards);
    if (possibleAttacks & kingToMoveLocation) {
        move.resultBits |= CHECK;
        move.resultBits &= ~PUSH; // undo the push bit
        return true;
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
        for (const auto& pieceName: {WK, WN, WB, WQ, WR, WP}) { if (canPieceEscapeCheck(pieceName)) { return true; } }
    } else {
        for (const auto& pieceName: {BK, BN, BB, BQ, BR, BP}) { if (canPieceEscapeCheck(pieceName)) { return true; } }
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
    if (tryMove(move)) {
        undoMove(move);
        return true;
    }
    return false;
}

bool BoardManager::handleCapture(Move& move) const{
    const auto capturedPiece = bitboards.getPiece(move.rankTo, move.fileTo);

    if (!capturedPiece.has_value()) {
        std::cout << "Error: No piece captured" << std::endl;
        return false;
    }

    if (capturedPiece.value() == WK || capturedPiece.value() == BK) { return false; }

    move.capturedPiece = capturedPiece.value();
    move.resultBits |= CAPTURE;
    return true;
}

void BoardManager::handleEnPassant(Move& move){
    move.resultBits |= EN_PASSANT;
    move.resultBits |= CAPTURE;
    const auto relevantFile = move.fileTo;
    const auto rankOffset = move.piece == WP ? -1 : 1;
    move.capturedPiece = bitboards.getPiece(move.rankTo + rankOffset, relevantFile).value();
}


bool BoardManager::lastTurnInCheck(const Move& move){
    const Piece lastTurnPiece = currentTurn == BLACK ? WK : BK;
    const Bitboard& kingLocation = bitboards[lastTurnPiece];

    // Iterate through the pieces that can attack it
    for (const auto& pieceName: filteredPieces[currentTurn]) {
        auto startingBoard = bitboards[pieceName];
        while (startingBoard) {
            // count trailing zeros to find the index of the first set bit
            const int startSquare = std::countr_zero(startingBoard);
            startingBoard &= startingBoard - 1;
            Bitboard prelimAttacks = 0ULL;

            // see if they can even attack in theory
            rules.getPseudoAttacks(pieceName, startSquare, prelimAttacks);
            if (!(kingLocation & prelimAttacks))
                continue;
            Bitboard possibleMoves = 0ULL;
            magicBitBoards.getMoves(startSquare, pieceName, bitboards, possibleMoves);
            if (possibleMoves & kingLocation) { return true; }
        }
    }

    return false;
}

std::string BoardManager::getFullFen(){
    auto baseFen = bitboards.toFEN();
    auto enPassantSquareString = (boardStateHistory.top().enPassantSquare == -1)
                                     ? "-"
                                     : Fen::squareToFen(boardStateHistory.top().enPassantSquare);
    auto resultFen = baseFen + (currentTurn == WHITE ? " w" : " b") + " KQkq " + enPassantSquareString + " 0 1";
    return resultFen;
}