//
// Created by jacks on 23/08/2025.
//

#include "../../include/GUI/EvaluationBar.h"

#include <iomanip>
#include <ios>
#include <string>
#include  <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>

EvaluationBar::EvaluationBar(const Vec2D& pos, const Vec2D& size, TTF_Font* font) : barSize_(size), barPos_(pos),
    font_(font){}


void EvaluationBar::draw(SDL_Renderer* renderer){
    // draw the white bar on the left
    const Vec2D whiteBarSize = {barSize_.x * evaluation_, barSize_.y};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const auto whiteRect = SDL_FRect{.x = barPos_.x, .y = barPos_.y, .w = whiteBarSize.x, .h = whiteBarSize.y};
    SDL_RenderFillRect(renderer, &whiteRect);

    // then draw the black bar on the right
    const Vec2D blackBarSize = {barSize_.x - whiteBarSize.x, barSize_.y};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    const auto blackRect = SDL_FRect{
                .x = barPos_.x + whiteBarSize.x, .y = barPos_.y, .w = blackBarSize.x, .h = blackBarSize.y
            };
    SDL_RenderFillRect(renderer, &blackRect);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << evaluation_;
    std::string evalText = ss.str();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font_,
                                                    evalText.c_str(),
                                                    evalText.size(),
                                                    SDL_Color{255, 0, 0, 255});

    const int textW = textSurface->w;
    const int textH = textSurface->h;
    const SDL_FRect textRect = {
                .x = barPos_.x + 5.0f,
                .y = barPos_.y + (barSize_.y - textH) / 2.0f,
                .w = static_cast<float>(textW),
                .h = static_cast<float>(textH)
            };

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
    SDL_DestroySurface(textSurface);
}