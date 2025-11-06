#ifndef OPENINGBOOK_H
#define OPENINGBOOK_H
#include <random>
#include <filesystem>
#include <fstream>

#include "Utility/Fen.h"

class OpeningBook {
public:

    OpeningBook()
        : rng(std::chrono::system_clock::now().time_since_epoch().count()){
        obFile = std::ifstream(OPENING_BOOK_DIR);
        std::string line;
        while (std::getline(obFile, line)) { openingPositions.push_back(line); }
    }


    FenString GetRandomFen(){
        int line = rng() % openingPositions.size();
        return openingPositions[line];
    }

private:

    std::mt19937 rng;
    std::ifstream obFile;
    std::vector<FenString> openingPositions;
};


#endif