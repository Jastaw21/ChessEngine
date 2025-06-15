//
// Created by jacks on 12/06/2025.
//

#ifndef DRAWABLEENTITY_H
#define DRAWABLEENTITY_H

#include <SDL3/SDL.h>

using SDLVec2D = SDL_Point;

struct RenderInfo {
    enum RenderType {
        RECT,
        CIRCLE,
        SPRITE
    };

    RenderType type;
    SDL_Color color;
    SDL_Point location;
    SDLVec2D size;
};

class DrawableEntity {
public:
    virtual ~DrawableEntity() = default;
    DrawableEntity();
    explicit DrawableEntity(const RenderInfo& render_info);

    virtual RenderInfo& getRenderInfo();
    virtual void draw(SDL_Renderer* renderer) = 0;

protected:
    RenderInfo renderInfo{};
};



#endif //DRAWABLEENTITY_H
