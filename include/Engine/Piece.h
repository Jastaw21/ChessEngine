//
// Created by jacks on 18/06/2025.
//

#ifndef PIECE_H
#define PIECE_H
#include <string>
#include <unordered_map>

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
    PIECE_N
};

enum Colours {
    WHITE,
    BLACK,
};

inline std::unordered_map<char, Piece> pieceMap = {
            {'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK},
            {'p', BP}, {'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK},
        };
inline std::unordered_map<Piece, char> reversePieceMap = {
            {WP, 'P'}, {WN, 'N'}, {WB, 'B'}, {WR, 'R'}, {WQ, 'Q'}, {WK, 'K'},
            {BP, 'p'}, {BN, 'n'}, {BB, 'b'}, {BR, 'r'}, {BQ, 'q'}, {BK, 'k'},
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

inline std::unordered_map<Piece, Colours> pieceColours = {
            {WP, WHITE},
            {WN, WHITE},
            {WB, WHITE},
            {WR, WHITE},
            {WQ, WHITE},
            {WK, WHITE},
            {BP, BLACK},
            {BN, BLACK},
            {BB, BLACK},
            {BR, BLACK},
            {BQ, BLACK},
            {BK, BLACK},
        };

class ConcretePiece {
public:

    ConcretePiece(const Piece piece, const int rank, const int file) : piece(piece), rank(rank), file(file){}

    Piece piece;

    int rank;
    int file;
};


#endif //PIECE_H
