//
// Created by jacks on 12/06/2025.
//

#ifndef DRAWABLEENTITY_H
#define DRAWABLEENTITY_H

#include <SDL3/SDL.h>

#include "Utility/Vec2D.h"

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
    DrawableEntity(const RenderInfo& render_info, const Vec2D& parentOffset);

    virtual RenderInfo &getRenderInfo();
    virtual void draw(SDL_Renderer* renderer) = 0;

protected:

    DrawableEntity* parentEntity_ = nullptr;
    Vec2D parentOffset_;
    RenderInfo renderInfo{};

public:

    [[nodiscard]] Vec2D getParentOffset() const{ return parentOffset_; }
    void set_parent_offset(const Vec2D& parent_offset){ parentOffset_ = parent_offset; }
};


#endif //DRAWABLEENTITY_H