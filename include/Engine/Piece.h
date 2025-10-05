//
// Created by jacks on 18/06/2025.
//

#ifndef PIECE_H
#define PIECE_H
#include <string>
#include <unordered_map>
#include <array>

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NO_PIECE,
};

enum Piece {
    WP, //0
    WN, //1
    WB, //2
    WR, //3
    WQ, //4
    WK, //5
    BP, //6
    BN, //7
    BB, //8
    BR, //9
    BQ, //10
    BK, //11
    PIECE_N //12
};

enum Colours {
    WHITE,
    BLACK,
};

inline std::unordered_map<char, Piece> CHAR_TO_PIECE_MAP = {
            {'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK},
            {'p', BP}, {'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK},
        };
inline std::unordered_map<Piece, char> PIECE_TO_CHAR_MAP = {
            {WP, 'P'}, {WN, 'N'}, {WB, 'B'}, {WR, 'R'}, {WQ, 'Q'}, {WK, 'K'},
            {BP, 'p'}, {BN, 'n'}, {BB, 'b'}, {BR, 'r'}, {BQ, 'q'}, {BK, 'k'},
        };


inline std::unordered_map<Colours, std::array<Piece, 6> > filteredPieces = {
            {WHITE, {WP, WN, WB, WR, WQ, WK}},
            {BLACK, {BP, BN, BB, BR, BQ, BK}}
        };

inline std::unordered_map<Piece, std::string> pieceNames = {
            {WP, "White Pawn"},
            {WN, "White Knight"},
            {WB, "White Bishop"},
            {WR, "White Rook"},
            {WQ, "White Queen"},
            {WK, "White King"},
            {BP, "Black Pawn"},
            {BN, "Black Knight"},
            {BB, "Black Bishop"},
            {BR, "Black Rook"},
            {BQ, "Black Queen"},
            {BK, "Black King"},
        };

// inline std::unordered_map<Piece, Colours> pieceColours = {
//             {WP, WHITE},
//             {WN, WHITE},
//             {WB, WHITE},
//             {WR, WHITE},
//             {WQ, WHITE},
//             {WK, WHITE},
//             {BP, BLACK},
//             {BN, BLACK},
//             {BB, BLACK},
//             {BR, BLACK},
//             {BQ, BLACK},
//             {BK, BLACK},
//         };

inline std::array<Colours, PIECE_N> pieceColours = {
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK

        };

inline std::unordered_map<Piece, PieceType> pieceTypes = {
            {WP, PieceType::PAWN},
            {WR, PieceType::ROOK},
            {WN, PieceType::KNIGHT},
            {WB, PieceType::BISHOP},
            {WQ, PieceType::QUEEN},
            {WK, PieceType::KING},
            {BP, PieceType::PAWN},
            {BR, PieceType::ROOK},
            {BN, PieceType::KNIGHT},
            {BB, PieceType::BISHOP},
            {BQ, PieceType::QUEEN},
            {BK, PieceType::KING},
        };

class ConcretePiece {
public:

    ConcretePiece(const Piece piece, const int rank, const int file) : piece(piece), rank(rank), file(file){}

    Piece piece;

    int rank;
    int file;
};

#endif //PIECE_H