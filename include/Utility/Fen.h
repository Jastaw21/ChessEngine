//
// Created by jacks on 15/06/2025.
//

#ifndef FEN_H
#define FEN_H
#include <string>


namespace Fen {
    inline std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    inline std::string STARTING_FEN_TEST = "r7/8/8/8/8/8/8/8";

    inline std::string rankAndFileToFen(const int rank, const int file){
        std::string returnString;
        const char rankString = 'a' - 1 + rank;
        returnString += rankString;
        returnString += std::to_string(file);
        return returnString;
    };

    inline void FenToRankAndFile(const std::string& fen, int& rank, int& file){
        file = fen[0] - 'a' + 1;
        rank = fen[1] - '1' + 1;
    };
}
#endif //FEN_H
