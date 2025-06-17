//
// Created by jacks on 15/06/2025.
//

#ifndef FEN_H
#define FEN_H
#include <unordered_map>

namespace Fen {
    inline std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    inline std::string STARTING_FEN_TEST = "r7/8/8/8/8/8/8/8";

    enum Piece {
        WP, WN, WB, WR, WQ, WK,
        BP, BN, BB, BR, BQ, BK,
        PIECE_N
    };

    inline std::unordered_map<char, Piece> pieceMap = {
        {'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK},
        {'p', BP}, {'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK},
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



}
#endif //FEN_H
