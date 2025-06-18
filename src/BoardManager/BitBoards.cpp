//
// Created by jacks on 16/06/2025.
//

#include "BoardManager/BitBoards.h"
#include <bitset>
#include "Engine/Piece.h"


BitBoards::BitBoards() {
    bitboards.fill(0ULL);

    // build the ranks and files for comparison
    for (int i = 0; i < 8; i++) {
        ranks[i] = buildRankBoard(i + 1);
    }
    for (char c = 'a'; c <= 'h'; c++) {
        files[c] = buildFileBoard(c);
    }
}

void BitBoards::loadFEN(const std::string &fen) {
    fen_ = fen;
    bitboards.fill(0ULL);
    // starting from a8, h8 is 63
    int square = 56;
    size_t i = 0;

    while (i < fen.size() && fen[i] != ' ') {
        char c = fen[i];

        if (c == '/')
            square -= 16; // Move to the next rank
        else if (isdigit(c))
            square += (c - '0'); // Skip that many empty squares
        else {
            auto it = pieceMap.find(c);
            if (it != pieceMap.end()) {
                bitboards[it->second] |= (1ULL << square);
            }
            square++;
        }
        i++;
    }
}

uint64_t BitBoards::getBitboard(const Piece &piece) const {
    return bitboards[piece];
}

void BitBoards::printBitboard() const {
    for (int i = 0; i < Piece::PIECE_N; ++i) {
        const auto piece = static_cast<Piece>(i);
        std::bitset<64> bits = bitboards[i];
        std::cout << "Piece " << pieceNames[piece] << bits << "\n";
    }
}

std::string &BitBoards::toFEN() {
    fen_ = "";
    // rank by rank
    for (int rank = 7; rank >= 0; --rank) {
        // will be used to count empty squares
        int numEmpty = 0;

        // file by file
        for (int file = 0; file < 8; ++file) {
            // square i.e., the bit we'll search
            const int square = rank * 8 + file;

            bool isPieceHere = false;

            // check each piece
            for (const auto &[key, value]: pieceMap) {
                // if the bit is in the bitboard, and-ed with the square, then we have a piece here
                if (bitboards[value] & (1ULL << square)) {
                    isPieceHere = true;
                    fen_ += key;
                    break;
                }
            }
            if (!isPieceHere)
                numEmpty++;
        }

        if (numEmpty > 0) {
            fen_ += std::to_string(numEmpty);
            numEmpty = 0;
        }
        if (!rank == 0)
            fen_ += '/';
    }


    return fen_;
}
