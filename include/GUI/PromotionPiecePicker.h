//
// Created by jacks on 25/08/2025.
//

#ifndef CHESS_PROMOTIONPIECEPICKER_H
#define CHESS_PROMOTIONPIECEPICKER_H
#include "DrawableEntity.h"
#include "VisualBoard.h"
#include "Engine/Piece.h"


class VisualBoard;


class PromotionPiecePicker : public DrawableEntity {
public:

    PromotionPiecePicker(VisualBoard* parent) : parent_(parent){
        actualPickerRect = {
                    pickerDimensions.x + parent->getParentOffset().x, pickerDimensions.y + parent_->getParentOffset().y,
                    pickerDimensions.w, pickerDimensions.h
                };
    }

    virtual void draw(SDL_Renderer* renderer) override;

    void setShouldDraw(bool value, Colours colour){
        shouldDraw_ = value;
        colourToDraw_ = colour;
    }

    void setShouldDraw(const bool value){ shouldDraw_ = value; }
    bool isBeingDrawn() const{ return shouldDraw_; }
    Piece getClickedPiece(const int xScreen, const int yScreen) const;

private:

    VisualBoard* parent_;
    bool shouldDraw_ = false;
    Colours colourToDraw_ = WHITE;
    std::array<Piece, 4> whitePieces = {WN, WQ, WB, WR};
    std::array<Piece, 4> blackPieces = {BN, BQ, BB, BR};

    SDL_FRect pickerDimensions{50, 50, 64 * 4, 64};
    SDL_FRect actualPickerRect;
    Vec2D imageSize = {64, 64};
};
#endif //CHESS_PROMOTIONPIECEPICKER_H