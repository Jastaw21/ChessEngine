//
// Created by jacks on 05/06/2025.
//

#ifndef BOARDBACKGROUND_H
#define BOARDBACKGROUND_H

#include "DrawableEntity.h"
#include "VisualPiece.h"
#include <memory>
#include <optional>

#include "Utility/Vec2D.h"

class BitBoards;
class Gui;

class BoardSquare final : public DrawableEntity {
public:

    BoardSquare(bool isWhite_p, int rank_p, int file_p, const SDL_FRect& rect);

    bool isWhite;

    void draw(SDL_Renderer* renderTarget) override;

    bool isDrawn() const{ return bIsDrawn; };
    void setIsDrawn(const bool value){ bIsDrawn = value; };

private:

    int rank;
    int file;
    SDL_FRect rect_;


    bool bIsDrawn = true;
};


class VisualBoard final : public DrawableEntity {
public:

    ~VisualBoard() override;

    VisualBoard(const Vec2D& boardSizePixels, ChessGui* gui);
    void build_background(const Vec2D& square_size);


    virtual void draw(SDL_Renderer* renderer) override;

    Vec2D squareSize() const;
    [[nodiscard]] const Vec2D &boardSize() const{ return boardSize_; }

    std::shared_ptr<VisualPiece> pieceAtLocation(int rank, int file) const;
    void updatePieceLocation(Piece piece, size_t index);

    bool board_dirty() const{ return boardDirty; }
    void set_board_dirty(const bool board_dirty){ boardDirty = board_dirty; }

private:

    std::vector<BoardSquare> squares_;
    std::vector<std::shared_ptr<VisualPiece> > pieces_{};

    void updatePieceLocations();
    Vec2D boardSize_;

    ChessGui* parent_;

    bool boardDirty = true;

    std::optional<std::shared_ptr<VisualPiece> > getFirstPieceOfType(const Piece piece_) const;
    std::unordered_map<Piece, VisualPiece> pieceInstances;
};


#endif //BOARDBACKGROUND_H
