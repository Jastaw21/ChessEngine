//
// Created by jacks on 05/06/2025.
//

#include "../include/VisualBoard.h"

#include <iostream>

#include "VisualPiece.h"


BoardSquare::BoardSquare(const bool isWhite_p, const int rank_p, const int file_p, const SDL_FRect &rect)
    : isWhite(isWhite_p),
      rank(rank_p),
      file(file_p),
      rect_(rect) {
    renderInfo.color = isWhite ? SDL_Color(255, 242, 204, 255) : SDL_Color(51, 48, 40, 255);
}

void BoardSquare::draw(SDL_Renderer *renderTarget) {
    if (!bIsDrawn)
        return;
    SDL_Color &color = renderInfo.color;
    SDL_SetRenderDrawColor(renderTarget, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderTarget, &rect_);
}

void VisualBoard::build_background(const Vec2D &square_size) {
    bool isWhite = true;

    for (int rank = 0; rank < 8; rank++) {
        isWhite = !isWhite;
        for (int file = 0; file < 8; file++) {
            const float inverse_rank = 8 - rank - 1;
            const float y_pos = inverse_rank * square_size.y;
            const float x_pos = file * square_size.x;

            SDL_FRect rect{.x = x_pos, .y = y_pos, .w = square_size.x, .h = square_size.y};
            BoardSquare square(isWhite, rank, file, rect);
            squares_.push_back(square);

            isWhite = !isWhite;
        }
    }
}

VisualBoard::VisualBoard(const Vec2D &boardSizePixels, ChessGui *gui): boardSize(boardSizePixels),
                                                                       parent_(gui) {
    // set the builder up
    const auto square_size = Vec2D(boardSizePixels.x / 8.f, boardSizePixels.y / 8.f);


    build_background(square_size);

    auto builder = VisualPieceBuilder(square_size, gui);
    auto created_pieces = builder.FromFEN();
    for (auto &piece: created_pieces) {
        pieces_.push_back(piece);
    }
}

VisualBoard::~VisualBoard() {
    while (!pieces_.empty()) { pieces_.pop_back(); };
    while (!squares_.empty()) { squares_.pop_back(); };
}

void VisualBoard::draw(SDL_Renderer *renderer) {
    for (auto &square: squares_) {
        square.draw(renderer);
    }

    for (auto &piece: pieces_) {
        piece->draw(renderer);
    }
}


std::shared_ptr<VisualPiece> VisualBoard::pieceAtLocation(int rank, const int file) {
    const float starting_x = file * squareSize().x;
    const float end_x = starting_x + squareSize().x;

    const float starting_y = rank * squareSize().y;
    const float end_y = starting_y + squareSize().y;


    for (const auto &piece: pieces_) {
        // Iterate over shared_ptr<VisualPiece>
        const bool xMatch = (starting_x <= piece->getLocation().x) && (piece->getLocation().x <= end_x);

        if (const bool yMatch = (starting_y <= piece->getLocation().y) && (piece->getLocation().y <= end_y); xMatch && yMatch) {
            return piece; // Return the shared_ptr
        }
    }
    return nullptr;
}

Vec2D VisualBoard::squareSize() const {
    const Vec2D local_size = boardSize / 8.f;
    return local_size;
}

std::vector<std::shared_ptr<VisualPiece> > pieces_{};
