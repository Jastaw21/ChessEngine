//
// Created by jacks on 06/10/2025.
//
#include "BoardManager/Move.h"

#include "BoardManager/BitBoards.h"

#include "Utility/Vec2D.h"

Move::Move(const Piece& piece, const int fromSquare, const int toSquare) : piece(piece),
                                                                           rankFrom(squareToRank(fromSquare)),
                                                                           fileFrom(squareToFile(fromSquare)),
                                                                           rankTo(squareToRank(toSquare)),
                                                                           fileTo(squareToFile(toSquare)){}

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

Move createMove(const Piece& piece, const RankAndFile squareFrom, const RankAndFile squareTo,
                const Piece promotedPiece){
    Move returnMove;
    returnMove.piece = piece;
    returnMove.rankFrom = squareFrom.rank;
    returnMove.fileFrom = squareFrom.file;
    returnMove.rankTo = squareTo.rank;
    returnMove.fileTo = squareTo.file;
    returnMove.promotedPiece = promotedPiece;

    return returnMove;
}