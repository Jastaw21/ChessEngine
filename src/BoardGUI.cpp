//
// Created by jacks on 05/06/2025.
//

#include "../include/BoardGUI.h"
#include "PieceGUI.h"

BoardSquare::BoardSquare(const bool isWhite_p, const int rank_p, const int file_p, const SDL_FRect &rect)
    : isWhite(isWhite_p),
      rank(rank_p),
      file(file_p),
      rect_(rect) {
    renderInfo.color = isWhite ? SDL_Color(255, 242, 204, 255) : SDL_Color(51, 48, 40, 255);
}

void BoardSquare::draw(SDL_Renderer *renderTarget) {
    SDL_Color &color = renderInfo.color;
    SDL_SetRenderDrawColor(renderTarget, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderTarget, &rect_);
}

void BoardGUI::build_background(const SDL_FPoint square_size) {
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

BoardGUI::BoardGUI(const SDLVec2D &boardSizePixels, ChessGui *gui): boardSize(boardSizePixels),
                                                                    parent_(gui) {
    // set the builder up
    const auto square_size = SDL_FPoint(boardSizePixels.x / 8.f, boardSizePixels.y / 8.f);


    build_background(square_size);

    auto builder = VisualPieceBuilder(square_size, gui);
    auto created_pieces = builder.FromFEN();
    for (std::shared_ptr<PieceGUI> piece: created_pieces) {
        pieces_.push_back(piece);
    }
}

void BoardGUI::draw(SDL_Renderer *renderer) {
    for (auto &square: squares_) {
        square.draw(renderer);
    }

    for (auto &piece: pieces_) {
        piece->draw(renderer);
    }
}
