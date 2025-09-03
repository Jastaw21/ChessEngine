//
// Created by jacks on 23/08/2025.
//

#include "../../include/GUI/EvaluationBar.h"

#include <iomanip>
#include <ios>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <sstream>

EvaluationBar::EvaluationBar(const Vec2D& pos, const Vec2D& size, TTF_Font* font) : barSize_(size), barPos_(pos),
    font_(font){}


void EvaluationBar::draw(SDL_Renderer* renderer){
    if (renderer_ == nullptr) { renderer_ = renderer; }
    // draw the white bar on the left
    const Vec2D whiteBarSize = {barSize_.x * evaluation_, barSize_.y};
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    const auto whiteRect = SDL_FRect{.x = barPos_.x, .y = barPos_.y, .w = whiteBarSize.x, .h = whiteBarSize.y};
    SDL_RenderFillRect(renderer_, &whiteRect);

    // then draw the black bar on the right
    const Vec2D blackBarSize = {barSize_.x - whiteBarSize.x, barSize_.y};
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    const auto blackRect = SDL_FRect{
                .x = barPos_.x + whiteBarSize.x, .y = barPos_.y, .w = blackBarSize.x, .h = blackBarSize.y
            };
    SDL_RenderFillRect(renderer_, &blackRect);

    SDL_RenderTexture(renderer_, renderedText, nullptr, &textRect);
}

void EvaluationBar::set_evaluation(const float evaluation){
    this->evaluation_ = evaluation;

    if (renderer_ == nullptr) { return; }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << evaluation_;
    std::string evalText = ss.str();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font_,
                                                    evalText.c_str(),
                                                    evalText.size(),
                                                    SDL_Color{255, 0, 0, 255});

    const int textW = textSurface->w;
    const int textH = textSurface->h;
    textRect = {
                .x = barPos_.x + 5.0f,
                .y = barPos_.y + (barSize_.y - textH) / 2.0f,
                .w = static_cast<float>(textW),
                .h = static_cast<float>(textH)
            };

    renderedText = SDL_CreateTextureFromSurface(renderer_, textSurface);
    SDL_DestroySurface(textSurface);
}