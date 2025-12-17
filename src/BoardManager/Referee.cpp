//
// Created by jacks on 06/10/2025.
//

#include "BoardManager/Referee.h"

#include "BoardManager/BitBoards.h"
#include "BoardManager/Move.h"

#include "Engine/ProcessChessEngine.h"


bool Referee::moveIsLegal(Move& move, BitBoards& boardState, MagicBitBoards& mbb, const int enPassantSquare){
    return validateMove(move, boardState, mbb, enPassantSquare);
}

int Referee::checkBoardStatus(BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                              Colours colourToMove){
    int result = 0;

    if (isKingInCheck(bitboards, magicBitBoards, colourToMove)) {
        if (colourToMove == WHITE)
            result |= BoardStatus::WHITE_CHECK;
        else
            result |= BoardStatus::BLACK_CHECK;
    }

    if (isKingInCheck(bitboards, magicBitBoards, colourToMove == WHITE ? BLACK : WHITE)) {
        if (colourToMove == WHITE)
            result |= BoardStatus::BLACK_CHECK;
        else
            result |= BoardStatus::WHITE_CHECK;
    }

    // no checks - can't be checkmate
    if (result == 0) { return BoardStatus::NORMAL; }

    if (!hasLegalMoveToEscapeCheck(bitboards, magicBitBoards, colourToMove)) {
        if (colourToMove == WHITE)
            result |= BoardStatus::WHITE_CHECKMATE;
        else
            result |= BoardStatus::BLACK_CHECKMATE;
    }

    return result;
}

bool Referee::hasAnyLegalMoves(BitBoards& bitBoards, MagicBitBoards& magicBitBoards, Colours colourToMove){

}

bool Referee::boardIsInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards, Colours colourToMove){
    return isKingInCheck(bitboards, magicBitBoards, colourToMove);
}


bool Referee::handleCapture(Move& move, const BitBoards& bitboards){
    const auto capturedPiece = bitboards.getPiece(move.rankTo, move.fileTo);

    if (capturedPiece.value() == WK || capturedPiece.value() == BK) { return false; }

    move.capturedPiece = capturedPiece.value();
    move.resultBits |= CAPTURE;
    return true;
}

void Referee::handleEnPassant(Move& move){
    move.resultBits |= EN_PASSANT;
    move.resultBits |= CAPTURE;
    move.capturedPiece = move.piece == WP ? BP : WP;
}

bool Referee::validateMove(Move& move, const BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                           const int enPassantSquare){
    // where are we going to?
    const int toSquare = rankAndFileToSquare(move.rankTo, move.fileTo);
    const Bitboard toSquareBitboard = 1ULL << toSquare;

    // can't go on top of a friendly piece
    const auto friendlyColour = move.piece <= 5 ? WHITE : BLACK;
    const auto friendlyPieces = bitboards.getOccupancy(friendlyColour);
    if (toSquareBitboard & friendlyPieces) {
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    const int fromSquare = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const auto enemyPieces = bitboards.getOccupancy() & ~friendlyPieces;

    // just get all the possible moves, if the to square doesn't appear in all, the move is therefore not any form of legal
    const auto allMoves = magicBitBoards.getMoves(fromSquare, move.piece, bitboards);
    if (!(allMoves & toSquareBitboard)) {
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    // we are attacking an enemy, check it's legal (i.e. non-king)
    if (toSquareBitboard & enemyPieces) {
        const bool captureHandled = handleCapture(move, bitboards);
        if (!captureHandled) {
            move.resultBits = 0;
            move.resultBits |= ILLEGAL_MOVE;
            return false;
        }
        // if it's not a promotion capture - escape now
        if (move.promotedPiece == PIECE_N) { return true; }
    }

    if (move.piece == WP || move.piece == BP) {
        if (enPassantSquare == toSquare) {
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

    // promotion
    if (move.promotedPiece != PIECE_N) {
        if (validatePromotion(move)) {
            move.resultBits |= PROMOTION;
            return true;
        }
        move.resultBits |= ILLEGAL_MOVE;
        return false;
    }

    if (const auto castlingMoves
                = magicBitBoards.getCastling(fromSquare, move.piece, bitboards);
        toSquareBitboard & castlingMoves
    ) {
        move.resultBits |= CASTLING;
        return true;
    }

    // all special cases handled, just a push move
    move.resultBits |= PUSH;
    return true;
}

bool Referee::validatePromotion(const Move& move){
    if (pieceColours[move.piece] != pieceColours[move.promotedPiece]) { return false; }

    if (move.promotedPiece != WQ && move.promotedPiece != BQ && move.promotedPiece != WR &&
        move.promotedPiece != BR && move.promotedPiece != WN && move.promotedPiece != BN && move.promotedPiece != WB
        && move.promotedPiece != BB) { return false; }

    return true;
}

bool Referee::isKingInCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards, Colours currentTurn){
    // will send out attackers of this turn colour to all squares they could possibly attack
    // and do some caching to avoid expensive multiple checks

    const auto kingToMove = currentTurn == WHITE ? WK : BK;
    const auto opposingPawn = (kingToMove == WK) ? BP : WP;
    const auto ourPawn = (kingToMove == WK) ? WP : BP;

    // Use static arrays to avoid repeated construction
    static constexpr std::array<Piece, 5> whiteAttackers{WN, WQ, WR, WB, WK};
    static constexpr std::array<Piece, 5> blackAttackers{BN, BQ, BR, BB, BK};

    // fake real relates to matching colours with the king of interest. fake = same colour
    const auto& fakeAttackers = (kingToMove == WK) ? whiteAttackers : blackAttackers;
    const auto& realAttackers = (kingToMove == BK) ? whiteAttackers : blackAttackers;

    const auto kingToMoveLocation = bitboards[kingToMove];
    if (!kingToMoveLocation) { return false; } // king is not on the board
    const auto kingToMoveSquare = std::countr_zero(kingToMoveLocation);

    Bitboard possibleAttacks = 0ULL;

    // the pawns are the only directional pieces, so have to check the colours the "right" way around
    possibleAttacks |= ourPawn == WP
                           ? magicBitBoards.rules.whitePawnAttacks.at(kingToMoveSquare)
                           : magicBitBoards.rules.blackPawnAttacks.at(kingToMoveSquare);
    if (possibleAttacks & bitboards.getOccupancy(opposingPawn)) { return true; }

    // precalc where they are to avoid some double checking
    auto allAttackerOccupancy = bitboards.getOccupancy(currentTurn == WHITE ? BLACK : WHITE);

    // a place to cache the pieces that could give a theoretical check, to save properly testing everything
    std::array<Piece, 6> piecesThatGiveCheck; // only need six pieces
    std::ranges::fill(piecesThatGiveCheck, PIECE_N);

    //if we can get the reachable starting point of attackers, we can narrow down the number of squares to launch full tests on their attack moves from
    std::array<uint64_t, PIECE_N> realAttackerMask;
    std::ranges::fill(realAttackerMask, 0ULL);

    bool anyPiecesRequireFurtherCheck = false;

    int pieceIndex = 0; // used to sync the fake->real attacker lookups
    for (const auto& pieceName: fakeAttackers) {
        const auto pseudoAttacks = magicBitBoards.rules.getPseudoAttacks(
            pieceName, kingToMoveSquare);

        // no collision with any enemy piece
        if (!(pseudoAttacks & allAttackerOccupancy)) {
            pieceIndex++;
            continue;
        }

        const auto realAttacker = realAttackers[pieceIndex];
        const auto realAttackerLocation = bitboards.getOccupancy(realAttacker);
        if (pseudoAttacks & realAttackerLocation) {
            anyPiecesRequireFurtherCheck = true;
            piecesThatGiveCheck[pieceIndex] = realAttacker; // cache which pieces so we only have to verify those
            realAttackerMask[realAttacker] = pseudoAttacks & realAttackerLocation;
            // cache the location of the attacker so we can avoid double checking
        }
        pieceIndex++;
    }

    if (anyPiecesRequireFurtherCheck) {
        for (const auto& pieceName: piecesThatGiveCheck) {
            if (pieceName == PIECE_N) continue; // this piece can't possibly attack us

            // get it's real attacks
            possibleAttacks |= magicBitBoards.findAttacksForPiece(pieceName, bitboards, realAttackerMask[pieceName]);
            if (possibleAttacks & kingToMoveLocation) { return true; }
        }
    }

    return false;
}

bool Referee::hasLegalMoveToEscapeCheck(BitBoards& bitboards, MagicBitBoards& magicBitBoards, Colours currentTurn){
    if (currentTurn == WHITE) {
        // king first - most likely to escape check I guess?
        for (const auto& pieceName: {WN, WK, WQ, WR, WB, WP}) {
            if (canPieceEscapeCheck(pieceName, bitboards, magicBitBoards, currentTurn)) { return true; }
        }
    } else {
        for (const auto& pieceName: {BN, BK, BQ, BR, BB, BP}) {
            if (canPieceEscapeCheck(pieceName, bitboards, magicBitBoards, currentTurn)) { return true; }
        }
    }
    return false;
}


bool Referee::isValidEscapeMove(Move& move, BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                const Colours currentTurn){
    if (!validateMove(move, bitboards, magicBitBoards, currentTurn)) { return false; } // move not even pseudolegal

    // now we need to check the board state for check mates etc
    bitboards.applyMove(move);

    // if the move results in, or leaves our king in check we can't do it.
    if (isKingInCheck(bitboards, magicBitBoards, currentTurn)) {
        bitboards.undoMove(move);
        return false;
    }

    bitboards.undoMove(move);
    return true;
}

bool Referee::hasValidMoveFromSquare(Piece pieceName, int startSquare, Bitboard destinationSquares,
                                     BitBoards& bitboards,
                                     MagicBitBoards& magicBitBoards, const Colours currentTurn){
    while (destinationSquares) {
        // count trailing zeros to find the index of the first set bit
        const int destinationSquare = std::countr_zero(destinationSquares);
        destinationSquares &= ~(1ULL << destinationSquare);
        auto move = Move(pieceName, startSquare, destinationSquare);

        if (isValidEscapeMove(move, bitboards, magicBitBoards, currentTurn)) { return true; }
    }

    return false;
}

bool Referee::canPieceEscapeCheck(const Piece& pieceName, BitBoards& bitboards, MagicBitBoards& magicBitBoards,
                                  const Colours currentTurn){
    auto startingBoard = bitboards[pieceName];
    while (startingBoard) {
        // count trailing zeros to find the index of the first set bit
        const int startSquare = std::countr_zero(startingBoard);
        startingBoard &= startingBoard - 1;
        const auto possibleMoves = magicBitBoards.getMoves(startSquare, pieceName, bitboards);
        if (hasValidMoveFromSquare(pieceName, startSquare, possibleMoves, bitboards, magicBitBoards, currentTurn)) {
            return true;
        }
    }
    return false;
}