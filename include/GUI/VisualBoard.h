//
// Created by jacks on 05/06/2025.
//

#ifndef BOARDBACKGROUND_H
#define BOARDBACKGROUND_H

#include <memory>
#include "DrawableEntity.h"
#include "VisualPiece.h"
#include "Utility/Vec2D.h"

struct RankAndFile;
class BitBoards;
class Gui;

enum class HighlightType {
    NONE,
    RECENT_MOVE,
    INVALID_MOVE,
    POSSIBLE_SQUARE
};

class BoardSquare final : public DrawableEntity {
public:

    BoardSquare(bool isWhite_p, int rank_p, int file_p, const SDL_FRect& rect);

    bool isWhite;

    void draw(SDL_Renderer* renderTarget) override;

    bool isDrawn() const{ return bIsDrawn; };
    void setIsDrawn(const bool value){ bIsDrawn = value; }
    void setHighlighted(const bool value){ isHiglighted = value; }
    bool isHighlighted() const{ return isHiglighted; }
    HighlightType getHighlightType() const{ return highlightType; }
    void setHighlightType(const HighlightType value){ highlightType = value; }

    RankAndFile getRankFile() const;

private:

    int rank;
    int file;
    SDL_FRect rect_;

    bool isHiglighted = false;
    HighlightType highlightType = HighlightType::NONE;
    bool bIsDrawn = true;
};


class VisualBoard final : public DrawableEntity {
public:

    ~VisualBoard() override;
    void drawPieces(SDL_Renderer* renderer);

    VisualBoard(const Vec2D& boardSizePixels, ChessGui* gui);
    VisualBoard(const Vec2D& boardSizePixels, ChessGui* gui, const Vec2D& parentOffset);
    void buildBackground(const Vec2D& square_size);
    void highlightSquare(RankAndFile rankAndFile);
    void highlightSquare(RankAndFile rankAndFile, HighlightType highlightType);
    void clearHighlights();
    void pickUpPiece(const RankAndFile& rankAndFile);
    void pickUpPiece(const RankAndFile& rankAndFile, const Piece& pieceHeld);
    void dropPiece();
    void updateHeldPieceLocation(const Vec2D mousePos){ heldPiecePosition_ = mousePos; };
    VisualPieceSet &getPieceSet(){ return pieceSet_; }

    virtual void draw(SDL_Renderer* renderer) override;

    Vec2D squareSize() const;
    [[nodiscard]] const Vec2D &boardSize() const{ return boardSize_; }

private:

    std::vector<BoardSquare> squares_;
    std::vector<std::shared_ptr<VisualPiece> > pieces_{};


    Vec2D boardSize_;

    ChessGui* parent_;
    Vec2D parentOffset_;

    VisualPieceSet pieceSet_;

    RankAndFile heldPieceOrigin_{};
    Piece heldPieceType = PIECE_N;
    Vec2D heldPiecePosition_{};
};


#endif //BOARDBACKGROUND_H