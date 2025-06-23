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
#include "Engine/Piece.h"
#include "Utility/Fen.h"
#include "Utility/Vec2D.h"

class ChessGui;


class VisualPiece final : public DrawableEntity {
public:

    ~VisualPiece() override;

    [[nodiscard]] Piece &getPiece() const;

    VisualPiece(std::shared_ptr<SDL_Texture> texture,
                const Vec2D& squareSize,
                std::shared_ptr<ConcretePiece> piece);

    virtual void draw(SDL_Renderer* renderer) override;
    void draw(SDL_Renderer* renderer, const SDL_FRect& destRect) const;

    void setLocation(int rank, int file);

    [[nodiscard]] Vec2D getLocation() const;

    [[nodiscard]] int getRank() const{ return concretePiece_->rank; }
    [[nodiscard]] int getFile() const{ return concretePiece_->file; }

private:

    std::shared_ptr<SDL_Texture> texture_;
    Vec2D squareSize_{};
    std::shared_ptr<ConcretePiece> concretePiece_;

    const Piece &getPieceType() const;
};


class VisualPieceBuilder {
public:

    explicit VisualPieceBuilder(const Vec2D& squareSize, ChessGui* gui);

    std::vector<std::shared_ptr<VisualPiece> > FromFEN(const std::string& FEN = Fen::STARTING_FEN);

    Vec2D squareSize;

    std::vector<std::shared_ptr<VisualPiece> > buildInstances();

private:

    std::string source_path = "C:/Users/jacks/source/repos/Chess/pieces/pieces-basic-png/";
    ChessGui* gui;
};


#endif //VISUAL_PIECE_H