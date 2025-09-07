//
// Created by jacks on 12/06/2025.
//


#include "GUI/VisualPiece.h"

#include <iostream>
#include <map>
#include <utility>
#include <SDL3_image/SDL_image.h>

#include "GUI/gui.h"

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
                         const Vec2D& squareSize, Piece piece
)

    : texture_(std::move(texture)), squareSize_(squareSize), piece_(piece){}

VisualPiece::~VisualPiece(){
    SDL_DestroyTexture(texture_.get());
    texture_.reset();
}


void VisualPiece::draw(SDL_Renderer* renderer, const SDL_FRect& destRect) const{
    const auto texture = texture_.get();
    SDL_RenderTexture(renderer, texture, &SRC_RECT_128_128, &destRect);
}


const std::shared_ptr<VisualPiece>& VisualPieceSet::operator[](Piece pieceToGet){
    const auto result = std::ranges::find_if(pieces_, [&](auto& visualPiece) {
        return visualPiece->piece() == pieceToGet;
    });

    return *result;
}

void VisualPieceSet::buildPieces(Vec2D squareSize, ChessGui* gui){
    for (int piece = 0; piece < PIECE_N; piece++) {
        const auto pieceType = static_cast<Piece>(piece);

        auto concretePiece = std::make_shared<ConcretePiece>(pieceType, 0, 0);

        const bool isWhite = pieceColours[pieceType] == WHITE;
        char fenStyleString = tolower(PIECE_TO_CHAR_MAP.at(pieceType));
        // where is the piece saved?
        std::string path = source_path +
                           (isWhite ? "white" : "black") +
                           pieceFileSuffixes.at(fenStyleString);

        // load the texture and make a pointer of it
        SDL_Texture* rawPieceTexture = IMG_LoadTexture(gui->getRenderer(), path.c_str());
        auto uniqueTexture = std::shared_ptr<SDL_Texture>(rawPieceTexture, SDL_DestroyTexture);

        // construct the piece
        auto visualPiece = std::make_shared < VisualPiece > (uniqueTexture, squareSize, pieceType);

        // add it and move on
        pieces_.push_back(visualPiece);
    }
}