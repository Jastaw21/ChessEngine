//
// Created by jacks on 05/10/2025.
//

#ifndef CHESS_EPDPARSER_H
#define CHESS_EPDPARSER_H

#include <string>
#include <fstream>
#include <vector>

#include "BoardManager/BoardManager.h"

#include "Utility/Fen.h"

struct EDPEntry {
    FenString fen;
    bool BestMove;
    std::string movePGN;
    Move move;
};

inline Piece getPiece(const std::string& move, Colours colourToMove){
    switch (move.at(0)) {
        case 'Q':
            return colourToMove == Colours::WHITE ? Piece::WQ : Piece::BQ;
        case 'R':
            return colourToMove == Colours::WHITE ? Piece::WR : Piece::BR;
        case 'K':
            return colourToMove == Colours::WHITE ? Piece::WK : Piece::BK;
        case 'B':
            return colourToMove == Colours::WHITE ? Piece::WB : Piece::BB;
        case 'N':
            return colourToMove == Colours::WHITE ? Piece::WN : Piece::BN;
        default:
            break;
    }

    // pawns don't specify position
    return colourToMove == Colours::WHITE ? Piece::WP : Piece::BP;
}

inline RankAndFile getDestinationOfMove(const std::string& PGNMove, const Piece pieceMoving){
    RankAndFile result;

    // if it was a capture will be like cxd4 or Bxd6
    if (PGNMove.at(1) == 'x') {
        result.file = PGNMove.at(2) - 'a' + 1;
        result.rank = PGNMove.at(3) - '0';
    } else {
        // non-pawn captures are just a4 e7 etc
        if (pieceMoving == Piece::WP || pieceMoving == Piece::BP) {
            result.file = PGNMove.at(0) - 'a' + 1;
            // and went to the specified rank
            result.rank = PGNMove.at(1) - '0';
        }

        // non-pawn, non captures are like Ba6
        else {
            result.file = PGNMove.at(1) - 'a' + 1;
            result.rank = PGNMove.at(2) - '0';
        }
    }

    return result;
}

inline RankAndFile inferStartingPointOfMove(const FenString& position, const std::string& PGNMove,
                                            const Piece pieceMoving){
    // will need to use some of the utilities in here
    auto bm = BoardManager();
    bm.setFullFen(position);

    RankAndFile result;

    // pawns will always have the column from in the first slot
    if (pieceMoving == Piece::WP || pieceMoving == Piece::BP) {
        result.file = PGNMove.at(0) - 'a' + 1;
        char rankToChar;

        if (PGNMove.at(1) == 'x') {
            // capture, so the to rank is in the 4th pos, index 3, i.e dxe6
            rankToChar = PGNMove.at(3);
        } else { rankToChar = PGNMove.at(1); }
        int rankToValue = rankToChar - '0';

        // white pawns always increase, black decrease, however we need to check if they moved two from the first rank
        if (pieceMoving == Piece::WP) {
            if (rankToValue == 4) {
                auto pawnOnStartingRank = bm.getBitboards()->getPiece(2, result.file);
                if (pawnOnStartingRank.has_value() && pawnOnStartingRank.value() == Piece::WP) { result.rank = 2; }
            } else { result.rank = rankToValue - 1; }
        }
        if (pieceMoving == Piece::BP) {
            if (rankToValue == 5) {
                auto pawnOnStartingRank = bm.getBitboards()->getPiece(7, result.file);
                if (pawnOnStartingRank.has_value() && pawnOnStartingRank.value() == Piece::BP) { result.rank = 7; }
            } else { result.rank = rankToValue + 1; }
        }
        return result;
    }

    auto destination = getDestinationOfMove(PGNMove, pieceMoving);
    auto startingPoints = bm.getBitboards()->getOccupancy(pieceMoving);
    while (startingPoints) {
        auto lsb = std::countr_zero(startingPoints);
        startingPoints &= startingPoints - 1;
        auto startSquare = squareToRankAndFileStruct(lsb);

        auto testMove = createMove(pieceMoving, startSquare, destination);
        if (bm.checkMove(testMove)) { return startSquare; }
    }

    return result;
}

inline Move PGNToUCI(const std::string& PGNMove, const FenString& Fen, Colours colourToMove){
    auto movingPiece = getPiece(PGNMove, colourToMove);
    const auto startSquare = inferStartingPointOfMove(Fen, PGNMove, movingPiece);
    const auto destinationSquare = getDestinationOfMove(PGNMove, movingPiece);
    auto move = createMove(movingPiece, Fen::rankAndFileToFen(startSquare) + Fen::rankAndFileToFen(destinationSquare));
    return move;
}


inline std::vector<EDPEntry> ParseEPDFile(std::string epd){
    std::ifstream file(epd);

    std::vector<EDPEntry> lines;
    std::string line;

    while (std::getline(file, line)) {
        std::string fenPosition;
        std::string fenColour;
        std::string fenEP;
        std::string fenCastling;
        std::string moveType;
        std::string move;
        std::istringstream iss(line);
        iss >> fenPosition >> fenColour >> fenEP >> fenCastling >> moveType >> move;

        // get rid of the trailing semi-colon in some lines
        if (move.back() == ';') { move.pop_back(); }
        EDPEntry thisEntry;

        thisEntry.fen = fenPosition + " " + fenColour + " " + fenEP + " " + fenCastling + " 1 0";
        thisEntry.BestMove = moveType == "bm";
        thisEntry.movePGN = move;
        thisEntry.move = PGNToUCI(move, thisEntry.fen, fenColour == "w" ? Colours::WHITE : Colours::BLACK);

        lines.push_back(thisEntry);
    }

    return lines;
}


#endif //CHESS_EPDPARSER_H