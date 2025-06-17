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


class VisualPiece final : public DrawableEntity {
public:
    VisualPiece(std::shared_ptr<SDL_Texture> texture, int rank, int file, const Vec2D &squareSize, const char type);
    ~VisualPiece() override;

    void draw(SDL_Renderer *renderer) override;


    int test = 0;

    void setLocation(const Vec2D &location);
    const Vec2D& getLocation() const { return location_; }

    bool logChanged = false;

    const char getType() const { return type_; }

private:
    std::shared_ptr<SDL_Texture> texture_;
    SDL_FRect srcRect{};
    Vec2D size_;
    Vec2D location_;

    const char type_;
};


class VisualPieceBuilder {
public:
    explicit VisualPieceBuilder(const Vec2D &squareSize, ChessGui *gui);

    std::vector<std::shared_ptr<VisualPiece> > FromFEN(const std::string &FEN = Fen::STARTING_FEN);

    Vec2D squareSize;

private:
    std::string source_path = "C:/Users/jacks/source/repos/Chess/pieces/pieces-basic-png/";
    ChessGui *gui;
};


#endif //VISUAL_PIECE_H
