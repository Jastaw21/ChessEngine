//
// Created by jacks on 05/06/2025.
//


#include "GUI/VisualBoard.h"

#include <bitset>

#include "BoardManager/BitBoards.h"
#include "GUI/gui.h"
#include "GUI/VisualPiece.h"

SDL_FRect addOffset(const SDL_FRect& rect, const Vec2D& offset){
    return SDL_FRect{rect.x + offset.x, rect.y + offset.y, rect.w, rect.h};
}


BoardSquare::BoardSquare(const bool isWhite_p, const int rank_p, const int file_p, const SDL_FRect& rect)
    : isWhite(isWhite_p),
      rank(rank_p),
      file(file_p),
      rect_(rect){ renderInfo.color = isWhite ? SDL_Color{255, 242, 204, 255} : SDL_Color{71, 98, 90, 255}; }

void BoardSquare::draw(SDL_Renderer* renderTarget){
    const SDL_Color& color = isHighlighted() ? SDL_Color{210, 245, 127, 1} : renderInfo.color;
    SDL_SetRenderDrawColor(renderTarget, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderTarget, &rect_);
}

RankAndFile BoardSquare::getRankFile() const{ return {rank, file}; }

VisualBoard::~VisualBoard(){
    while (!pieces_.empty()) { pieces_.pop_back(); }
    while (!squares_.empty()) { squares_.pop_back(); }
}

void VisualBoard::drawPieces(SDL_Renderer* renderer) const{
    // uses the managers bitboards as the source of truth.
    const auto& boards = parent_->getMatchManager()->getBitboards();
    for (int pieceIndex = 0; pieceIndex < PIECE_N; pieceIndex++) {
        auto board = boards->getBitboard(static_cast<Piece>(pieceIndex));

        while (board) {
            const auto square = std::countr_zero(board);
            board &= ~(1ULL << square);
            int rank;
            int file;
            squareToRankAndFile(square, rank, file);
            const int invertedRank = 8 - rank;
            const int invertedFile = file - 1;
            const auto destRect = SDL_FRect{
                        .x = invertedFile * squareSize().x, .y = invertedRank * squareSize().y, .w = squareSize().x,
                        .h = squareSize().y
                    };

            const auto offsetRect = addOffset(destRect, parentOffset_);
            pieces_[pieceIndex]->draw(renderer, offsetRect);
        }
    }
}

VisualBoard::VisualBoard(const Vec2D& boardSizePixels, ChessGui* gui) : boardSize_(boardSizePixels),
                                                                        parent_(gui){
    // set the builder up
    const auto square_size = Vec2D{
                boardSizePixels.x / 8.f, boardSizePixels.y / 8.f
            };

    buildBackground(square_size);

    auto builder = VisualPieceBuilder(square_size, gui);
    const auto created_pieces = builder.buildInstances();
    for (auto& piece: created_pieces) { pieces_.push_back(piece); }
}

VisualBoard::VisualBoard(const Vec2D& boardSizePixels, ChessGui* gui,
                         const Vec2D& parentOffset) : boardSize_(boardSizePixels),
                                                      parent_(gui), parentOffset_(parentOffset){
    // set the builder up
    const auto square_size = Vec2D{boardSizePixels.x / 8.f, boardSizePixels.y / 8.f};

    buildBackground(square_size);

    auto builder = VisualPieceBuilder(square_size, gui);
    const auto created_pieces = builder.buildInstances();
    for (auto& piece: created_pieces) { pieces_.push_back(piece); }
}

void VisualBoard::buildBackground(const Vec2D& square_size){
    bool isWhite = true;

    for (int rank = 0; rank < 8; rank++) {
        isWhite = !isWhite;
        for (int file = 0; file < 8; file++) {
            const float inverse_rank = 8 - rank - 1;
            const float y_pos = inverse_rank * square_size.y;
            const float x_pos = file * square_size.x;

            SDL_FRect rect{.x = x_pos, .y = y_pos, .w = square_size.x, .h = square_size.y};
            auto offsetRect = addOffset(rect, parentOffset_);
            BoardSquare square(isWhite, rank, file, offsetRect);
            squares_.push_back(square);

            isWhite = !isWhite;
        }
    }
}

void VisualBoard::highlightSquare(RankAndFile rankAndFile){
    auto highlightedSquare = std::ranges::find_if(squares_, [&](auto& square) {
        return square.getRankFile().file + 1 == rankAndFile.file && square.getRankFile().rank + 1 == rankAndFile.rank;
    });

    highlightedSquare->setHighlighted(true);
}

void VisualBoard::clearHighlights(){ for (auto& square: squares_) { square.setHighlighted(false); } }

void VisualBoard::draw(SDL_Renderer* renderer){
    // draw squares
    for (auto& square: squares_) { square.draw(renderer); }

    drawPieces(renderer);
}

Vec2D VisualBoard::squareSize() const{
    const Vec2D local_size = boardSize_ / 8.f;
    return local_size;
}




