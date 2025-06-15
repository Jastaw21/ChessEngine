//
// Created by jacks on 12/06/2025.
//

#include "PieceGUI.h"

#include <map>
#include <iostream>
#include <SDL3_image/SDL_image.h>

#include <utility>
#include "gui.h"

const inline std::map<const char, std::string> pieceFileSuffixes = {
    {'p', "-pawn.png"},
    {'n', "-knight.png"},
    {'b', "-bishop.png"},
    {'r', "-rook.png"},
    {'q', "-queen.png"},
    {'k', "-king.png"}
};


PieceGUI::PieceGUI(std::shared_ptr<SDL_Texture> texture, const int rank, const int file,
                   const SDL_FPoint &squareSize) {
    texture_ = std::move(texture);

    const int inverseRank = rank;

    destRect = SDL_FRect{
        static_cast<float>(file) * static_cast<float>(squareSize.x),
        static_cast<float>(inverseRank) * static_cast<float>(squareSize.y),
        static_cast<float>(squareSize.x),
        static_cast<float>(squareSize.y)
    };

    srcRect = SDL_FRect(0.f, 0.f, 128.f, 128.f);
}

void PieceGUI::draw(SDL_Renderer *renderer) {
    const auto text = texture_.get();
    SDL_RenderTexture(renderer, text, &srcRect, &destRect);
}

VisualPieceBuilder::VisualPieceBuilder(const SDL_FPoint &squareSize, ChessGui *gui) : squareSize(squareSize), gui(gui) {
}

std::vector<std::shared_ptr<PieceGUI> > VisualPieceBuilder::FromFEN(const std::string &FEN) {
    std::vector<std::shared_ptr<PieceGUI> > pieces;

    int rank = 0;
    int file = 0;

    for (const char c: FEN) {
        if (c == ' ') break;
        if (c == '/') {
            rank++;
            file = 0;
            continue;
        }

        if (isdigit(c)) {
            // char cheating - the digit will be x places ahead of 0 in the char mapping
            file += c - '0';
        } else {
            const bool isWhite = isupper(c);
            char type = tolower(c);

            std::string path = source_path + (isWhite ? "white" : "black") + pieceFileSuffixes.at(type);
            SDL_Texture *rawPieceTexture = IMG_LoadTexture(gui->getRenderer(), path.c_str());
            auto uniqueTexture = std::shared_ptr<SDL_Texture>(rawPieceTexture, SDL_DestroyTexture);


            pieces.push_back(std::make_unique<PieceGUI>(uniqueTexture, rank, file, squareSize));
            file++;
        }
    }

    return pieces;
}
