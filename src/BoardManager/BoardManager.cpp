//
// Created by jacks on 18/06/2025.
//

#include "BoardManager/BoardManager.h"
#include "BoardManager/BitBoards.h"

std::string Move::toUCI() const{
    std::string uci;
    const char fileFromChar = 'a' + fileFrom;
    uci += fileFromChar;
    uci += std::to_string(rankFrom);
    const char fileToChar = 'a' + fileTo;
    uci += fileToChar;
    uci += std::to_string(rankTo);

    return uci;
}


BoardManager::BoardManager() = default;

bool BoardManager::moveIsLegal(const Move& move){
    const int whiteRanksPossible = move.rankFrom == 2 ? 2 : 1;
    const int blackRanksPossible = move.rankFrom == 7 ? 2 : 1;
    const int deltaRank = move.rankTo - move.rankFrom;
    const int deltaFile = move.fileTo - move.fileFrom;

    switch (move.piece) {
        case Piece::WP:
            return move.rankTo <= move.rankFrom + whiteRanksPossible && move.fileFrom == move.fileTo;

        case Piece::BP:
            return move.rankTo >= move.rankFrom - blackRanksPossible && move.fileFrom == move.fileTo;

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

bool BoardManager::moveIsPossible(const Move& move){
    const auto moveString = move.toUCI();

    const bool fileInBounds = move.fileTo < 8 && move.fileTo >= 0;
    const bool rankInBounds = move.rankTo < 8 && move.rankTo >= 0;

    if (!fileInBounds || !rankInBounds)
        return false;

    return true;
}

bool BoardManager::moveDestinationIsEmpty(const Move& move) const{
    auto test = bitboards.getPiece(move.rankTo, move.fileTo);
    return !test.has_value();
}

bool BoardManager::moveDestinationIsOccupiedWithOpponent(const Move& move) const{
    const auto pieceThere = bitboards.getPiece(move.rankTo, move.fileTo);
    if (!pieceThere.has_value()) { return false; }

    if (pieceColours[pieceThere.value()] == pieceColours[move.piece])
        return false;

    return true;
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
    if (!prelimCheckMove(move)) {
        move.result = MoveResult::ILLEGAL_MOVE;
        return false;
    }

    const auto pieceColour = pieceColours[move.piece];
    const auto testColour = pieceColour == "White" ? "Black" : "White";

    // check if is capture
    if (moveDestOccupiedByColour(testColour, move)) {
        move.result = MoveResult::PIECE_CAPTURE;
        makeMove(move);
        return true;
    }
    move.result = MoveResult::MOVE_TO_EMPTY_SQUARE;
    makeMove(move);
    return true;
}

bool BoardManager::prelimCheckMove(Move& move) const{
    return moveIsLegal(move) &&
           moveIsPossible(move) &&
           (
               moveDestinationIsEmpty(move)
               || moveDestinationIsOccupiedWithOpponent(move)
           );
}

void BoardManager::makeMove(const Move& move){
    // set the from bit to zero
    auto squareFrom = (move.rankFrom - 1) * 8 + (move.fileFrom);
    bitboards[move.piece] &= ~(1ULL << squareFrom);

    // if it was a capture, set that piece to zero
    if (move.result == MoveResult::PIECE_CAPTURE)
        bitboards.setZero(move.rankTo, move.fileTo,move);

    // set the to bit to one
    auto squareTo = (move.rankTo - 1) * 8 + (move.fileTo);
    bitboards[move.piece] |= (1ULL << squareTo);


}
