//
// Created by jacks on 05/06/2025.
//

#ifndef BOARDBACKGROUND_H
#define BOARDBACKGROUND_H

#include "DrawableEntity.h"
#include "PieceGUI.h"
#include <memory>
#include <optional>

#include "Vec2D.h"

class Gui;

class BoardSquare final : public DrawableEntity {
public:
    BoardSquare(bool isWhite_p, int rank_p, int file_p, const SDL_FRect &rect);

    bool isWhite;

    void draw(SDL_Renderer *renderTarget) override;

private:
    int rank;
    int file;
    SDL_FRect rect_;
};


class BoardGUI final : public DrawableEntity {
public:
    void build_background(const Vec2D &square_size);

    BoardGUI(const Vec2D &boardSizePixels, ChessGui *gui);

    Vec2D boardSize;

    void draw(SDL_Renderer *renderer) override;

    std::shared_ptr<PieceGUI> pieceAtLocation(int rank, int file);

    Vec2D squareSize() const;

private:
    std::vector<BoardSquare> squares_;
    std::vector<std::shared_ptr<PieceGUI>> pieces_{};

    ChessGui *parent_;
};


#endif //BOARDBACKGROUND_H