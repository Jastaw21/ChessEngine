///
// Created by jacks on 18/06/2025.
//

// ReSharper disable CppTooWideScopeInitStatement
#include "BoardManager/BoardManager.h"

#include <bitset>
#include <iostream>
#include <vector>

#include "BoardManager/BitBoards.h"
#include "BoardManager/Rules.h"

std::string Move::toUCI() const{
    std::string uci;
    const char fileFromChar = 'a' + fileFrom - 1;
    uci += fileFromChar;
    uci += std::to_string(rankFrom);
    const char fileToChar = 'a' + fileTo - 1;
    uci += fileToChar;
    uci += std::to_string(rankTo);

    return uci;
}

Move createMove(const Piece& piece, const std::string& moveUCI){
    const char fromFile = moveUCI[0];
    const int fileFrom = fromFile - 'a' + 1;

    const char fromRank = moveUCI[1];
    const int rankFrom = fromRank - '1' + 1;

    const char toFile = moveUCI[2];
    const int fileTo = toFile - 'a' + 1;

    const char toRank = moveUCI[3];
    const int rankTo = toRank - '1' + 1;

    const Move newMove = {
                .piece = piece, .rankFrom = rankFrom, .fileFrom = fileFrom, .rankTo = rankTo, .fileTo = fileTo
            };

    return newMove;
}


BoardManager::BoardManager() = default;

bool BoardManager::opponentKingInCheck(Move& move){
    const auto& opponentKing = pieceColours[move.piece] == WHITE ? BK : WK;
    const auto& opponentKingLocation = bitboards[opponentKing];

    if (opponentKingLocation == 0) {
        return false; // No king on board
    }

    const auto attacks = RulesCheck::getAttackMoves(rankAndFileToSquare(move.rankTo, move.fileTo), move.piece,
                                                    &bitboards);

    if ((attacks & opponentKingLocation) != 0) {
        move.resultBits |= bCHECK;
        return true;
    }

    return false;
}

bool BoardManager::checkMove(Move& move){
    if (!prelimCheckMove(move)) { return false; }
    makeMove(move);
    if (friendlyKingInCheck(move)) {
        undoMove(move);
        move.resultBits |= bKING_IN_CHECK;
        return false;
    }
    if (opponentKingInCheck(move)) { move.resultBits |= bCHECK; }
    undoMove(move);
    return true;
}

bool BoardManager::prelimCheckMove(Move& move){
    move.resultBits = 0; // reset
    const int fromSquare = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const int toSquare = rankAndFileToSquare(move.rankTo, move.fileTo);

    uint64_t pushes;
    uint64_t rawAttacks;

    if (move.piece == WP || move.piece == BP) {
        const auto opponentColour = pieceColours[move.piece] == WHITE ? BLACK : WHITE;
        const auto allOccupiedSquares = bitboards.getOccupancy();
        auto opponentOccupiedSquares = bitboards.getOccupancy(opponentColour);

        const auto pseudoPushes = rules.getPseudoPawnPushes(move.piece, fromSquare);
        pushes = pseudoPushes & ~allOccupiedSquares;

        if (move.piece == WP) {
            if (move.rankFrom == 2 && bitboards.testSquare(rankAndFileToSquare(move.rankFrom + 1, move.fileFrom))) {
                pushes = 0ULL;
            }
        }
        if (move.piece == BP) {
            if (move.rankFrom == 7 && bitboards.testSquare(rankAndFileToSquare(move.rankFrom - 1, move.fileFrom))) {
                pushes = 0ULL;
            }
        }

        rawAttacks = rules.getPseudoPawnAttacks(move.piece, fromSquare) | pushes;
        rawAttacks &= opponentOccupiedSquares;
        const auto enPassantTargetSquares = rules.getPseudoPawnEP(move.piece, fromSquare, opponentOccupiedSquares);
        rawAttacks |= enPassantTargetSquares;
    }

    // boards
    else {
        pushes = RulesCheck::getPushMoves(fromSquare, move.piece, &bitboards);
        rawAttacks = RulesCheck::getAttackMoves(fromSquare, move.piece, &bitboards);
    }
    const auto castlingMoves = RulesCheck::getCastlingMoves(fromSquare, move.piece, &bitboards);
    const auto attacks = rawAttacks ^ pushes;

    if (castlingMoves && 1ULL << toSquare & castlingMoves) {
        move.resultBits |= bCASTLING;
        return true;
    }

    // check if it's a plain old push move
    if (pushes & 1ULL << rankAndFileToSquare(move.rankTo, move.fileTo)) {
        move.resultBits |= bPUSH;
        return true;
    }

    // have found an attack move
    if (attacks & 1ULL << rankAndFileToSquare(move.rankTo, move.fileTo)) {
        // see what we captured
        const auto capturedPiece = bitboards.getPiece(move.rankTo, move.fileTo);
        // if it's nothing, check if there's an en passant on
        if (!capturedPiece.has_value()) {
            // en passant check
            if (move.piece == WP || move.piece == BP) {
                // don't even bother checking if not on the right rank
                const auto attackableRank = pieceColours[move.piece] == WHITE ? 6 : 3;
                if (move.rankTo == attackableRank) {
                    // where is vulnerable
                    const auto enPassantVulnerableSquares =
                            RulesCheck::getEnPassantVulnerableSquares(&bitboards, pieceColours[move.piece]);

                    // this move is to an en passant viable square
                    if (enPassantVulnerableSquares & 1ULL << rankAndFileToSquare(move.rankTo, move.fileTo)) {
                        move.resultBits |= bEN_PASSANT;
                        return true;
                    }
                }

                // fall through to default
            }

            move.resultBits |= bILLEGAL_MOVE;
            return false;
        }
        // can't capture kings
        if (capturedPiece.value() == WK || capturedPiece.value() == BK) {
            move.resultBits |= bILLEGAL_MOVE;
            return false;
        }
        move.resultBits |= bCAPTURE;
        move.capturedPiece = capturedPiece.value();
        return true;
    }

    move.resultBits |= bILLEGAL_MOVE;
    return false;
}

bool BoardManager::tryMove(Move& move){
    if (!checkMove(move)) { return false; }
    makeMove(move);
    return true;
}

void BoardManager::makeMove(Move& move){
    if (move.resultBits & bCASTLING) {
        bitboards.setZero(move.rankFrom, move.fileFrom);
        bitboards.setOne(move.piece, move.rankTo, move.fileTo);

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
        bitboards.setOne(relevantRook, move.rankTo, movedRookFileTo);
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        bitboards.setZero(move.rankTo, movedRookFileFrom);
    }

    // set the "from" square of the moving piece to zero
    bitboards.setZero(move.rankFrom, move.fileFrom);

    // check to see if discovered capture
    if (const auto discoveredPiece = bitboards.getPiece(move.rankTo, move.fileTo);
        discoveredPiece.has_value() && pieceColours[discoveredPiece.value()] != pieceColours[move.piece]) {
        move.resultBits |= bCAPTURE;
        move.capturedPiece = discoveredPiece.value();
    }

    // if it was a capture, set that piece to zero
    if (move.resultBits & bCAPTURE)
        bitboards.setZero(move.rankTo, move.fileTo);

    // if it was an en_passant capture, set the correct square to zero
    if (move.resultBits & bEN_PASSANT) {
        const auto rankOffset = pieceColours[move.piece] == WHITE ? -1 : 1;
        bitboards.setZero(move.rankTo + rankOffset, move.fileTo);
    }

    // set the to square of the moving piece to one
    bitboards.setOne(move.piece, move.rankTo, move.fileTo);

    if (currentTurn == WHITE)
        currentTurn = BLACK;
    else
        currentTurn = WHITE;

    moveHistory.push(move);
}

void BoardManager::undoMove(const Move& move){
    // set the "from" bit back to one
    const auto squareFrom = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    bitboards[move.piece] |= 1ULL << squareFrom;

    // if it was a capture, restore that piece to one
    if (move.resultBits & bCAPTURE) {
        const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
        bitboards[move.capturedPiece] |= 1ULL << squareTo;
    }

    // if it was an en_passant capture, restore the correct square to one
    if (move.resultBits & bEN_PASSANT) {
        const auto opponentPawn = pieceColours[move.piece] == WHITE ? BP : WP;
        const auto rankOffset = pieceColours[move.piece] == WHITE ? -1 : 1;
        bitboards.setOne(opponentPawn, move.rankTo + rankOffset, move.fileTo);
    }

    // if it was a castling, restore the rooks to their original positions
    if (move.resultBits & bCASTLING) {
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

    // set the "to" bit back to zero for this piece
    const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
    bitboards[move.piece] &= ~(1ULL << squareTo);

    moveHistory.pop();

    if (currentTurn == WHITE)
        currentTurn = BLACK;
    else
        currentTurn = WHITE;
}

void BoardManager::undoMove(){
    if (moveHistory.empty())
        return;
    undoMove(moveHistory.top());
}

bool BoardManager::moveIsEnPassant(Move& move){
    if (abs(moveHistory.top().rankTo - moveHistory.top().rankFrom) != 2)
        return false;
    const uint64_t attackedSquare = 1ULL << rankAndFileToSquare(move.rankTo, move.fileTo);
    const auto& otherPawnPiece = pieceColours[move.piece] == WHITE ? BP : WP;
    const auto& locationsOfOtherPawns = bitboards[otherPawnPiece];
    const std::bitset<64> locationsOfOtherPawnsBits(locationsOfOtherPawns);
    int offset = pieceColours[move.piece] == WHITE ? 8 : -8;

    // track the locations of the opponent's pawns, shifted correctly
    uint64_t otherPawnLocations = 0ULL;
    for (int bit = 0; bit < locationsOfOtherPawnsBits.size(); ++bit) {
        if (locationsOfOtherPawnsBits.test(bit) && bit >= 8 && bit <= 55) {
            otherPawnLocations |= 1ULL << (bit + offset);
        }
    }

    const bool isEnPassant = (attackedSquare & otherPawnLocations) > 0 && abs(
                                 moveHistory.top().rankTo - moveHistory.top().rankFrom) == 2;

    if (!isEnPassant)
        return false;

    move.resultBits |= bEN_PASSANT;
    move.capturedPiece = otherPawnPiece;
    return true;
}

bool BoardManager::friendlyKingInCheck(const Move& move){
    const Colours friendlyColor = pieceColours[move.piece];
    const Piece friendlyKing = friendlyColor == BLACK ? BK : WK;
    const uint64_t kingLocation = bitboards[friendlyKing];

    if (kingLocation == 0) {
        return false; // No king on board
    }

    // Iterate through enemy pieces
    for (int pieceIndex = 0; pieceIndex < PIECE_N; pieceIndex++) {
        const auto pieceName = static_cast<Piece>(pieceIndex);
        if (pieceColours[move.piece] == pieceColours[pieceName])
            continue;

        const auto pieceLocations = getStartingSquaresOfPiece(pieceName);
        for (const int startSquare: pieceLocations) {
            // prelim check if anything could feasibly attack this square
            const uint64_t prelimAttacks = rules.getPseudoAttacks(pieceName, startSquare);
            if (!(kingLocation & prelimAttacks))
                continue;
            const uint64_t attackedSquares = RulesCheck::getAttackMoves(startSquare, pieceName, &bitboards);
            if ((attackedSquares & kingLocation) != 0) { return true; }
        }
    }

    return false;
}

std::vector<int> BoardManager::getStartingSquaresOfPiece(const Piece& piece){
    std::vector<int> startingSquares;

    const auto& pieceLocation = std::bitset<64>(bitboards[piece]);
    if (!pieceLocation.any())
        return startingSquares;

    for (int index = 0; index < pieceLocation.size(); ++index) {
        if (pieceLocation.test(index)) { startingSquares.push_back(index); }
    }
    return startingSquares;
}











