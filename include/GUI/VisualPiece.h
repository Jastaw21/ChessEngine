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
                const Vec2D& squareSize);
    virtual void draw(SDL_Renderer* renderer) override{ return; }

    void draw(SDL_Renderer* renderer, const SDL_FRect& destRect) const;

    void setLatched(const bool latched){ isLatched_ = latched; }
    bool isLatched() const{ return isLatched_; }

private:

    std::shared_ptr<SDL_Texture> texture_;
    Vec2D squareSize_{};


    const Piece &getPieceType() const;
    bool isLatched_ = false;
};


class VisualPieceBuilder {
public:

    explicit VisualPieceBuilder(const Vec2D& squareSize, ChessGui* gui);

    std::vector<std::shared_ptr<VisualPiece> > FromFEN(const std::string& FEN = Fen::STARTING_FEN);

    Vec2D squareSize;

    std::vector<std::shared_ptr<VisualPiece> > buildInstances();

private:

    std::string source_path = "C:/Users/jacks/source/repos/Chess/resources/pieces/pieces-basic-png/";
    ChessGui* gui;
};

class VisualPieceSet {
public:

    explicit VisualPieceSet(const Vec2D& squareSize, ChessGui* gui_);
    ~VisualPieceSet();

private:

    void buildPieces();


    std::vector<std::shared_ptr<VisualPiece> > pieces_;
    std::string source_path = "C:/Users/jacks/source/repos/Chess/resources/pieces/pieces-basic-png/";

    ChessGui* gui_;
    Vec2D squareSize_;
};


#endif //VISUAL_PIECE_H