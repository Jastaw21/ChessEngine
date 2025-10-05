//
// Created by jacks on 04/08/2025.
//

#include "../../include/Engine/ZobristHash.h"

#include <chrono>

#include "BoardManager/BoardManager.h"

char getCharFromPiece(const Piece piece){
    switch (piece) {
        case WP:
            return 'P';
        case WB:
            return 'B';
        case WR:
            return 'R';
        case WK:
            return 'K';
        case WQ:
            return 'Q';
        case WN:
            return 'N';

        case BP:
            return 'p';
        case BB:
            return 'b';
        case BR:
            return 'r';
        case BK:
            return 'k';
        case BQ:
            return 'q';
        case BN:
            return 'n';
        default:
            break;
    }

    return ' ';
}

void ZobristHash::initializeHashFromFen(FenString fenString){
    std::istringstream fenStream(fenString);

    std::string line;
    std::string fenPiecePlacement;
    std::string fenActiveColour;
    std::string fenCastling;
    std::string fenEnPassant;
    std::string fenHalfMoveClock;
    std::string fenFullMoveNumber;

    fenStream >> fenPiecePlacement >> fenActiveColour >> fenCastling >> fenEnPassant >> fenHalfMoveClock >>
            fenFullMoveNumber;

    // starting from a8
    int rank = 8;
    int file = 1;
    size_t i = 0;

    hashValue = 0;
    while (i < fenPiecePlacement.size() && fenPiecePlacement[i] != ' ') {
        if (char c = fenPiecePlacement[i]; c == '/') {
            // Move to the next rank
            rank--;
            file = 1;
        } else if (isdigit(c))
            file += c - '0'; // Skip that many empty squares
        else {
            auto randoms = getArray(c);
            auto square = (rank - 1) * 8 + (file - 1);
            hashValue ^= randoms[square];
            file++;
        }
        i++;
    }

    if (fenActiveColour == "b") { hashValue ^= blackToMove; }
}

ZobristHash::ZobristHash(const FenString& fenString){
    initZobrist();
    initializeHashFromFen(fenString);
}

void ZobristHash::setFen(const FenString& fenString){ initializeHashFromFen(fenString); }

void ZobristHash::addMove(const Move& move){
    // we'll use these all the way down
    const auto pieceReversed = getCharFromPiece(move.piece);
    const auto squareFrom = rankAndFileToSquare(move.rankFrom, move.fileFrom);
    const auto squareTo = rankAndFileToSquare(move.rankTo, move.fileTo);

    // XOR out where we moved from - this is true in any move.
    const auto& movingPieceArray = getArray(pieceReversed);
    hashValue ^= movingPieceArray[squareFrom];

    // used to avoid getting the wrong piece set in edge case/special moves like castling where the piece doesn't move to the real square
    bool shouldMoveTo = true;

    if (move.resultBits & MoveResult::EN_PASSANT) {
        const int rankOffset = (move.piece == WP) ? -1 : 1;
        const int enPassantCapturedSquare = rankAndFileToSquare(move.rankTo + rankOffset, move.fileTo);
        hashValue ^= (move.piece == WP) ? blackPawn[enPassantCapturedSquare] : whitePawn[enPassantCapturedSquare];
    } else if (move.resultBits & MoveResult::CAPTURE) {
        const auto& capturedArray = getArray(PIECE_TO_CHAR_MAP[move.capturedPiece]);
        hashValue ^= capturedArray[squareTo];
    }

    // treat pure capture separately
    else if (move.resultBits & MoveResult::CAPTURE) {
        // toggle the captured piece off
        const auto capturedArray = getArray(PIECE_TO_CHAR_MAP[move.capturedPiece]);
        hashValue ^= capturedArray[squareTo];
    }

    if (move.resultBits & MoveResult::CASTLING) {
        const auto relevantRook = move.piece == WK ? 'R' : 'r'; // what is the rook we also need to move?
        const auto& relevantArray = getArray(relevantRook);

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

        hashValue ^= relevantArray[rankAndFileToSquare(move.rankTo, movedRookFileTo)];
        hashValue ^= relevantArray[rankAndFileToSquare(move.rankTo, movedRookFileFrom)];
    }
    if (move.resultBits & MoveResult::PROMOTION) {
        shouldMoveTo = false; // we don't want to move our moving piece here
        const auto& promotedArray = getArray(PIECE_TO_CHAR_MAP[move.promotedPiece]);
        hashValue ^= promotedArray[squareTo]; // switch to the new piece
    }

    if (shouldMoveTo) { hashValue ^= movingPieceArray[squareTo]; }

    // always toggle the moves
    hashValue ^= blackToMove;
}

void ZobristHash::undoMove(const Move& move){ addMove(move); }

std::array<uint64_t, 64>& ZobristHash::getArray(const char pieceIndex){
    switch (pieceIndex) {
        case 'P':
            return whitePawn;
        case 'N':
            return whiteKnight;
        case 'B':
            return whiteBishop;
        case 'R':
            return whiteRook;
        case 'Q':
            return whiteQueen;
        case 'K':
            return whiteKing;
        case 'p':
            return blackPawn;
        case 'n':
            return blackKnight;
        case 'b':
            return blackBishop;
        case 'r':
            return blackRook;
        case 'q':
            return blackQueen;
        case 'k':
            return blackKing;
        default:
            return whitePawn;
    }
}

void ZobristHash::fillRandomArray(std::array<uint64_t, 64>& array){ for (auto& val: array) { val = rng(); } }

void ZobristHash::initZobrist(){
    rng.seed(seed);

    fillRandomArray(whitePawn);
    fillRandomArray(whiteKnight);
    fillRandomArray(whiteBishop);
    fillRandomArray(whiteRook);
    fillRandomArray(whiteQueen);
    fillRandomArray(whiteKing);

    fillRandomArray(blackPawn);
    fillRandomArray(blackKnight);
    fillRandomArray(blackBishop);
    fillRandomArray(blackRook);
    fillRandomArray(blackQueen);
    fillRandomArray(blackKing);

    blackToMove = rng();
}