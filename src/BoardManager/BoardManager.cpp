//
// Created by jacks on 18/06/2025.
//

#include "BoardManager/BoardManager.h"

#include <bitset>
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

Move createMove(const Piece& piece, const std::string& moveUCI){
    const char fromFile = moveUCI[0];
    const int fileFrom = (fromFile - 'a') + 1;

    const char fromRank = moveUCI[1];
    const int rankFrom = (fromRank - '1') + 1;

    const char toFile = moveUCI[2];
    const int fileTo = (toFile - 'a') + 1;

    const char toRank = moveUCI[3];
    const int rankTo = (toRank - '1') + 1;

    const Move newMove = {
                .piece = piece, .rankFrom = rankFrom, .fileFrom = fileFrom, .rankTo = rankTo, .fileTo = fileTo
            };

    return newMove;
}


BoardManager::BoardManager() = default;

bool BoardManager::moveIsLegalForPiece(const Move& move){
    const int deltaRank = move.rankTo - move.rankFrom;
    const int deltaFile = move.fileTo - move.fileFrom;

    switch (move.piece) {
        case Piece::WP:
            // if it doesn't move north at all, can't work
            if (deltaRank <= 0)
                return false;

            if (move.rankFrom == 2)
                return abs(deltaRank) <= 2;
            else
                return abs(deltaRank) <= 1;

        case Piece::BP:
            // if it doesn't move south at all, can't work
            if (deltaRank >= 0)
                return false;
            if (move.rankFrom == 7)
                return abs(deltaRank) <= 2;
            else
                return abs(deltaRank) <= 1;

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
    // knights can jump anywhere
    if (move.piece == Piece::BN || move.piece == Piece::WN)
        return false;
    std::vector<int> squaresOnPath;

    const int deltaRank = move.rankTo - move.rankFrom;
    const int deltaFile = move.fileTo - move.fileFrom;

    // single ranks and file moves can't pass over others
    if (abs(deltaRank) <= 1 && abs(deltaFile) <= 1)
        return false;

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

    // we now have the squares th`at we'll cross
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

bool BoardManager::kingInCheck(const Piece& piece, const Move& move){
    Piece validAttackingPieces[5];

    if (pieceColours[piece] == "Black") {
        validAttackingPieces[0] = Piece::WR;
        validAttackingPieces[1] = Piece::WN;
        validAttackingPieces[2] = Piece::WB;
        validAttackingPieces[3] = Piece::WQ;
        validAttackingPieces[4] = Piece::WK;
    } else {
        validAttackingPieces[0] = Piece::BR;
        validAttackingPieces[1] = Piece::BN;
        validAttackingPieces[2] = Piece::BB;
        validAttackingPieces[3] = Piece::BQ;
        validAttackingPieces[4] = Piece::BK;
    }

    const auto& kingLocation = bitboards[piece];
    const auto& kingBits = std::bitset<64>(kingLocation);
    int kRank, kFile;
    for (size_t index = 0; index < kingBits.size(); ++index) {
        if (kingBits.test(index)) { squareToRankAndFile(index, kRank, kFile); }
    }

    for (int attacker = 0; attacker < 5; attacker++) {
        const auto& relevantBoard = bitboards[validAttackingPieces[attacker]];
        const auto& bits = std::bitset<64>(relevantBoard);

        for (size_t index = 0; index < bits.size(); ++index) {
            if (bits.test(index)) {
                int rank, file;
                squareToRankAndFile(index, rank, file);
                Move move = {
                            .piece = validAttackingPieces[attacker],
                            .rankFrom = rank, .fileFrom = file,
                            .rankTo = kRank, .fileTo = kFile
                        };

                if (checkMove(move))
                    return true;
            }
        }
    }
    return false;
}

bool BoardManager::tryMove(Move& move){
    if (!checkMove(move)) { return false; }
    makeMove(move);
    return true;
}

bool BoardManager::captureIsLegal(Move& move){
    if (move.capturedPiece == Piece::WK || move.capturedPiece == Piece::BK) {
        move.capturedPiece = Piece::PIECE_N;
        return true;
    }

    // if it's a pawn, must be diagonal to capture
    if (move.piece == Piece::WP || move.piece == Piece::BP) {
        const bool isDiagonal = abs(move.rankTo - move.rankFrom) == 1 && abs(move.fileTo - move.fileFrom) == 1;
        return isDiagonal;
    }

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

    // is the square occupied by this colour?
    if (moveDestOccupiedByColour(pieceColours[move.piece], move)) {
        move.result = MoveResult::SQUARE_OCCUPIED;
        return false;
    }

    // would we have to step over other pieces?
    if (pieceInWay(move)) {
        move.result = MoveResult::BLOCKING_PIECE;
        return false;
    }

    if (move.piece == Piece::WK || move.piece == Piece::BK) {
        if (kingInCheck(move.piece, move)) {
            move.result = MoveResult::KING_IN_CHECK;
            return false;
        }
    }

    // would it be a capture?
    if (!moveDestinationIsEmpty(move)) {
        // check the capture is legal
        const auto capturedPiece = bitboards.getPiece(move.rankTo, move.fileTo);
        move.capturedPiece = capturedPiece.value();
        if (captureIsLegal(move)) {
            move.result = MoveResult::PIECE_CAPTURE;
            return true;
        } else {
            move.result = MoveResult::MOVE_NOT_LEGAL_FOR_PIECE;
            return false;
        }
    }

    // if it's not a capture and is a pawn, recheck that it's vertical
    if ((move.piece == Piece::WP || move.piece == Piece::BP)
        && move.fileTo != move.fileFrom) {
        move.result = MoveResult::MOVE_NOT_LEGAL_FOR_PIECE;
        return false;
    }

    // the last possible result is an empty square?
    move.result = MoveResult::MOVE_TO_EMPTY_SQUARE;
    return true;
}

void BoardManager::undoMove(const Move& move){
    // set the from bit back to one
    auto squareFrom = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    bitboards[move.piece] |= (1ULL << squareFrom);

    // if it was a capture, restore that piece to one
    if (move.result == MoveResult::PIECE_CAPTURE) {
        auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
        bitboards[move.capturedPiece] |= (1ULL << squareTo);
    }

    // set the to bit back to zero for this piece
    auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);
    bitboards[move.piece] &= ~(1ULL << squareTo);
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