//
// Created by jacks on 12/06/2025.
//

#include "VisualPiece.h"

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


VisualPiece::VisualPiece(std::shared_ptr<SDL_Texture> texture, const int rank, const int file,
                         const Vec2D &squareSize, const char type) : type_(type) {
    texture_ = std::move(texture);

    // put it in its default place
    const int inverseRank = rank;
    location_ = {
        .x = static_cast<float>(file) * static_cast<float>(squareSize.x),
        .y = static_cast<float>(inverseRank) * static_cast<float>(squareSize.y)
    };

    size_ = squareSize;

    srcRect = SDL_FRect(0.f, 0.f, 128.f, 128.f);;
}

VisualPiece::~VisualPiece() {
    SDL_DestroyTexture(texture_.get());
    texture_.reset();
}

void VisualPiece::draw(SDL_Renderer *renderer) {
    const auto text = texture_.get();
    const auto destRect = SDL_FRect{location_.x, location_.y, size_.x, size_.y};
    SDL_RenderTexture(renderer, text, &srcRect, &destRect);
}

void VisualPiece::setLocation(const Vec2D &location) {
    location_ = location;
}

VisualPieceBuilder::VisualPieceBuilder(const Vec2D &squareSize, ChessGui *gui) : squareSize(squareSize), gui(gui) {
}

std::vector<std::shared_ptr<VisualPiece> > VisualPieceBuilder::FromFEN(const std::string &FEN) {
    std::vector<std::shared_ptr<VisualPiece> > pieces;

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
            auto piece = std::make_shared<VisualPiece>(uniqueTexture, rank, file, squareSize, type);
            pieces.push_back(piece);
            file++;
        }
    }

    return pieces;
}
