//
// Created by jacks on 18/06/2025.
//

#include "BoardManager/BoardManager.h"

#include <vector>

#include "BoardManager/BitBoards.h"
#include "Utility/math.h"

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


BoardManager::BoardManager() = default;

bool BoardManager::moveIsLegalForPiece(const Move& move){
    const int whiteRanksPossible = move.rankFrom == 2 ? 2 : 1;
    const int blackRanksPossible = move.rankFrom == 7 ? 2 : 1;
    const int deltaRank = move.rankTo - move.rankFrom;
    const int deltaFile = move.fileTo - move.fileFrom;

    switch (move.piece) {
        case Piece::WP:
            return move.rankTo <= move.rankFrom + whiteRanksPossible && move.fileFrom == move.fileTo && deltaRank > 0;

        case Piece::BP:
            return move.rankTo >= move.rankFrom - blackRanksPossible && move.fileFrom == move.fileTo && deltaRank < 0;

        case Piece::WR:
        case Piece::BR:
            return move.rankFrom == move.rankTo || move.fileFrom == move.fileTo;

        case Piece::WB:
        case Piece::BB:
            return abs(deltaRank) == abs(deltaFile);

        case Piece::WQ:
        case Piece::BQ:
            return true;

        case Piece::WK:
        case Piece::BK:
            return abs(deltaRank) == 1 || abs(deltaFile) == 1;

        case Piece::WN:
        case Piece::BN:
            return abs(deltaRank) == 2 && abs(deltaFile) == 1 || abs(deltaRank) == 1 && abs(deltaFile) == 2;

        default:
            return true;
    };
}

bool BoardManager::moveInBounds(const Move& move){
    const auto moveString = move.toUCI();

    const bool fileInBounds = move.fileTo <= 8 && move.fileTo > 0;
    const bool rankInBounds = move.rankTo <= 8 && move.rankTo > 0;

    if (!fileInBounds || !rankInBounds)
        return false;

    return true;
}

bool BoardManager::pieceInWay(const Move& move) const{
    if (move.piece == Piece::BN || move.piece == Piece::WN)
        return false;
    std::vector<int> squaresOnPath;

    const int deltaRank = move.rankTo - move.rankFrom;
    const int deltaFile = move.fileTo - move.fileFrom;

    // moving horizontally
    if (deltaRank == 0) {
        for (int file = move.fileFrom + sign(deltaFile); file != move.fileTo; file += sign(deltaFile)) {
            int square = rankAndFileToSquare(move.rankFrom, file);
            squaresOnPath.push_back(square);
        }
    }
    // moving vertically
    else if (deltaFile == 0) {
        for (int rank = move.rankFrom + sign(deltaRank); rank != move.rankTo; rank += sign(deltaRank)) {
            int square = rankAndFileToSquare(rank, move.fileTo);
            squaresOnPath.push_back(square);
        }
    }

    // moving diagonally
    else {
        for (int step = 1; step <= deltaRank; step++) {
            const int newRank = move.rankFrom + step * sign(deltaRank);
            const int newFile = move.fileFrom + step * sign(deltaFile);
            int square = rankAndFileToSquare(newRank, newFile);
            squaresOnPath.push_back(square);
        }
    }

    // we now have the squares that we'll cross
    uint64_t crossedSquares = 0ULL;
    for (const auto& square: squaresOnPath) { crossedSquares |= (1ULL << square); }

    // return true if any collisions along route
    return bitboards.test(crossedSquares);
}

bool BoardManager::moveDestinationIsEmpty(const Move& move) const{
    auto test = bitboards.getPiece(move.rankTo, move.fileTo);
    return !test.has_value();
}

bool BoardManager::moveDestOccupiedByColour(const std::string& testColour, const Move& move) const{
    const auto test = bitboards.getPiece(move.rankTo, move.fileTo);
    // the square is empty
    if (!test.has_value()) { return false; }

    if ((pieceColours[test.value()] == testColour))
        return true;

    return false;
}

bool BoardManager::tryMove(Move& move){
    if (!checkMove(move)) { return false; }
    makeMove(move);
    return true;
}

bool BoardManager::checkMove(Move& move){
    // can this piece do that kind of move?
    if (!moveIsLegalForPiece(move)) {
        move.result = MoveResult::MOVE_NOT_LEGAL_FOR_PIECE;
        return false;
    }
    // is it in bounds?
    if (!moveInBounds(move)) {
        move.result = MoveResult::MOVE_OUT_OF_BOUNDS;
        return false;
    }

    if (pieceInWay(move)) {
        move.result = MoveResult::BLOCKING_PIECE;
        return false;
    }

    // is the square occupied by this colour?
    if (moveDestOccupiedByColour(pieceColours[move.piece], move)) {
        move.result = MoveResult::SQUARE_OCCUPIED;
        return false;
    }

    // is it an empty square?
    if (moveDestinationIsEmpty(move)) {
        move.result = MoveResult::MOVE_TO_EMPTY_SQUARE;
        return true;
    }

    // the last thing possible is a capture
    move.result = MoveResult::PIECE_CAPTURE;
    return true;
}


void BoardManager::makeMove(const Move& move){
    // set the from bit to zero
    auto squareFrom = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    bitboards[move.piece] &= ~(1ULL << squareFrom);

    // if it was a capture, set that piece to zero
    if (move.result == MoveResult::PIECE_CAPTURE)
        bitboards.setZero(move.rankTo, move.fileTo);

    // set the to bit to one
    auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
    bitboards[move.piece] |= (1ULL << squareTo);
}
