//
// Created by jacks on 15/06/2025.
//

#ifndef FEN_H
#define FEN_H
#include <string>


namespace Fen {
    inline const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    inline const std::string FULL_STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    inline const std::string STARTING_FEN_TEST = "r7/8/8/8/8/8/8/8";
    inline const std::string KIWI_PETE_FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R";
    inline const std::string POSITION_3_FEN = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8";

    inline std::string rankAndFileToFen(const int rank, const int file){
        std::string returnString;
        const char rankString = 'a' - 1 + rank;
        returnString += rankString;
        returnString += std::to_string(file);
        return returnString;
    }

    inline void FenToRankAndFile(const std::string& fen, int& rank, int& file){
        file = fen[0] - 'a' + 1;
        rank = fen[1] - '1' + 1;
    }

    inline int FenToSquare(const std::string& fen){
        const int file = fen[0] - 'a' + 1;
        const int rank = fen[1] - '1' + 1;

        const int startingSquareOfRank = (rank - 1) * 8;
        return startingSquareOfRank + (file - 1);
    }
}
#endif //FEN_H
