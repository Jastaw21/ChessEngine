//
// Created by jacks on 25/08/2025.
//

// ReSharper disable CppDFAConstantConditions
// ReSharper disable CppDFAUnreachableCode
#include "../../include/GUI/PromotionPiecePicker.h"

#include "../../include/GUI/VisualBoard.h"


void PromotionPiecePicker::draw(SDL_Renderer* renderer){
    if (!shouldDraw_)
        return;

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 1);
    SDL_RenderFillRect(renderer, &actualPickerRect);

    int xOffset = 0;

    for (const auto piece: colourToDraw_ == WHITE ? whitePieces : blackPieces) {
        SDL_FRect pieceRect{
                    .x = actualPickerRect.x + xOffset++ * imageSize.x, .y = actualPickerRect.y, .w = imageSize.x,
                    .h = imageSize.y
                };
        parent_->getPieceSet()[piece]->draw(renderer, pieceRect);
    }
}

Piece PromotionPiecePicker::getClickedPiece(const int xScreen, const int yScreen) const{
    // bounds check
    if (yScreen > actualPickerRect.y + actualPickerRect.h) return PIECE_N;
    if (yScreen < actualPickerRect.y) return PIECE_N;
    if (xScreen > actualPickerRect.x + actualPickerRect.w) return PIECE_N;
    if (xScreen < actualPickerRect.x) return PIECE_N;

    const int pixelsAcross = xScreen - actualPickerRect.x;
    const int stepsAcross = pixelsAcross / (imageSize.x);

    Piece returnValue;
    if (colourToDraw_ == WHITE) { returnValue = whitePieces.at(stepsAcross); } else {
        returnValue = blackPieces.at(stepsAcross);
    }

    return returnValue;
}