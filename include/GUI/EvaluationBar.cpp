//
// Created by jacks on 23/08/2025.
//

#include "EvaluationBar.h"


EvaluationBar::EvaluationBar(const Vec2D& pos, const Vec2D& size) : barSize_(size), barPos_(pos){}


void EvaluationBar::draw(SDL_Renderer* renderer){
    // draw the white bar on the left
    Vec2D whiteBarSize = {barSize_.x * evaluation_, barSize_.y};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    auto whiteRect = SDL_FRect{.x = barPos_.x, .y = barPos_.y, .w = whiteBarSize.x, .h = whiteBarSize.y};
    SDL_RenderFillRect(renderer, &whiteRect);

    // then draw the black bar on the right
    Vec2D blackBarSize = {barSize_.x - whiteBarSize.x, barSize_.y};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    auto blackRect = SDL_FRect{
                .x = barPos_.x + whiteBarSize.x, .y = barPos_.y, .w = blackBarSize.x, .h = blackBarSize.y
            };
    SDL_RenderFillRect(renderer, &blackRect);
}