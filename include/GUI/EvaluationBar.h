//
// Created by jacks on 23/08/2025.
//

#ifndef CHESS_EVALUATIONBAR_H
#define CHESS_EVALUATIONBAR_H

#include <SDL3_ttf/SDL_ttf.h>

#include "DrawableEntity.h"

struct TTF_Font;

class EvaluationBar : public DrawableEntity {
public:

    EvaluationBar(const Vec2D& pos, const Vec2D& size, TTF_Font* font);
    virtual void draw(SDL_Renderer* renderer) override;

private:

    float evaluation_ = 0.5f;
    Vec2D barSize_ = {800, 100};
    Vec2D barPos_ = {0, 0};

public:

    float get_evaluation() const{ return evaluation_; }
    void set_evaluation(const float evaluation);

    TTF_Font* font_ = nullptr;
    SDL_Texture* renderedText = nullptr;
    SDL_FRect textRect;
    SDL_Renderer* renderer_;
};


#endif //CHESS_EVALUATIONBAR_H