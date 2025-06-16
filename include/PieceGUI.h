//
// Created by jacks on 12/06/2025.
//

#ifndef VISUAL_PIECE_H
#define VISUAL_PIECE_H

#include <memory>

#include "DrawableEntity.h"
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include "Vec2D.h"

#include "Fen.h"

class ChessGui;


class PieceGUI final : public DrawableEntity {
public:
    PieceGUI(std::shared_ptr<SDL_Texture> texture, int rank, int file, const Vec2D &squareSize);

    void draw(SDL_Renderer *renderer) override;

    Vec2D location_;
    int test = 0;

    void setLocation(const Vec2D &location);

    bool logChanged = false;

private:
    std::shared_ptr<SDL_Texture> texture_;
    SDL_FRect srcRect{};
    Vec2D size_;
};


class VisualPieceBuilder {
public:
    explicit VisualPieceBuilder(const Vec2D &squareSize, ChessGui *gui);

    std::vector<std::shared_ptr<PieceGUI>> FromFEN(const std::string &FEN = Fen::STARTING_FEN);

    Vec2D squareSize;

private:
    std::string source_path = "C:/Users/jacks/source/repos/Chess/pieces/pieces-basic-png/";
    ChessGui *gui;
};


#endif //VISUAL_PIECE_H
