//
// Created by jacks on 12/06/2025.
//


#include <map>
#include <iostream>
#include <SDL3_image/SDL_image.h>
#include <utility>

#include "GUI/gui.h"
#include "GUI/VisualPiece.h"

const inline std::map<const char, std::string> pieceFileSuffixes = {
            {'p', "-pawn.png"},
            {'n', "-knight.png"},
            {'b', "-bishop.png"},
            {'r', "-rook.png"},
            {'q', "-queen.png"},
            {'k', "-king.png"}
        };

inline constexpr auto SRC_RECT_128_128 = SDL_FRect{0, 0, 128, 128};


VisualPiece::VisualPiece(std::shared_ptr<SDL_Texture> texture,
                         const int rank, const int file,
                         const Vec2D& squareSize,
                         std::shared_ptr<ConcretePiece> piece)

    : texture_(std::move(texture)), squareSize_(squareSize), concretePiece_(std::move(piece)){}

VisualPiece::~VisualPiece(){
    SDL_DestroyTexture(texture_.get());
    texture_.reset();
}

Piece &VisualPiece::getPiece() const{ return concretePiece_->piece; }

void VisualPiece::draw(SDL_Renderer* renderer){
    const auto text = texture_.get();
    const auto destRect = SDL_FRect{getLocation().x, getLocation().y, squareSize_.x, squareSize_.y};
    SDL_RenderTexture(renderer, text, &SRC_RECT_128_128, &destRect);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void VisualPiece::setLocation(const int rank, const int file){
    concretePiece_->rank = rank;
    concretePiece_->file = file;
}

Vec2D VisualPiece::getLocation() const{
    return Vec2D{
                .x = static_cast<float>(concretePiece_->file) * static_cast<float>(squareSize_.x),
                .y = static_cast<float>(8 - concretePiece_->rank) * static_cast<float>(squareSize_.y)
            };
}

const Piece &VisualPiece::getPieceType() const{ return concretePiece_->piece; }
VisualPieceBuilder::VisualPieceBuilder(const Vec2D& squareSize, ChessGui* gui) : squareSize(squareSize), gui(gui){}

std::vector<std::shared_ptr<VisualPiece> > VisualPieceBuilder::FromFEN(const std::string& FEN){
    std::vector<std::shared_ptr<VisualPiece> > pieces;

    int rank = 8;
    int file = 0;

    for (const char c: FEN) {
        if (c == ' ') break;
        if (c == '/') {
            rank--;
            file = 0;
            continue;
        }

        if (isdigit(c)) {
            // char cheating - the digit will be x places ahead of 0 in the char mapping
            file += c - '0';
        } else {
            // white always caps
            const bool isWhite = isupper(c);

            // reverse the char to get the piece type
            char fenType = tolower(c);

            Piece pieceEnum = pieceMap.at(c);
            auto concretePiece = std::make_shared<ConcretePiece>(pieceEnum, rank, file);

            // where is the piece saved?
            std::string path = source_path + (isWhite ? "white" : "black") + pieceFileSuffixes.at(fenType);

            // load the texture and make a pointer of it
            SDL_Texture* rawPieceTexture = IMG_LoadTexture(gui->getRenderer(), path.c_str());
            auto uniqueTexture = std::shared_ptr<SDL_Texture>(rawPieceTexture, SDL_DestroyTexture);

            // construct the piece
            auto piece = std::make_shared<VisualPiece>(uniqueTexture, rank, file, squareSize, concretePiece);

            // add it and move on
            pieces.push_back(piece);
            file++;
        }
    }

    return pieces;
}
