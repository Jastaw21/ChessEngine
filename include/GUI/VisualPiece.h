//
// Created by jacks on 12/06/2025.
//

#ifndef VISUAL_PIECE_H
#define VISUAL_PIECE_H

#include <memory>
#include <string>
#include <vector>
#include <SDL3/SDL.h>

#include "DrawableEntity.h"
#include "Utility/Vec2D.h"
#include "Engine/Piece.h"

class ChessGui;


class VisualPiece final : public DrawableEntity {
public:

    ~VisualPiece() override;
    VisualPiece(std::shared_ptr<SDL_Texture> texture,
                const Vec2D& squareSize, Piece piece);
    virtual void draw(SDL_Renderer* renderer) override{ return; }

    void draw(SDL_Renderer* renderer, const SDL_FRect& destRect) const;

    void setLatched(const bool latched){ isLatched_ = latched; }
    bool isLatched() const{ return isLatched_; }
    [[nodiscard]] Piece piece() const{ return piece_; }
    void set_piece(Piece piece){ piece_ = piece; }

private:

    std::shared_ptr<SDL_Texture> texture_;
    Vec2D squareSize_{};

    Piece piece_;

    bool isLatched_ = false;
};

class VisualPieceSet {
public:

    const std::shared_ptr<VisualPiece> &operator[](Piece pieceToGet);

    void buildPieces(Vec2D squareSize, ChessGui* gui);

private:

    std::vector<std::shared_ptr<VisualPiece> > pieces_{};
    std::string source_path = "C:/Users/jacks/source/repos/Chess/resources/pieces/pieces-basic-png/";
};


#endif //VISUAL_PIECE_H