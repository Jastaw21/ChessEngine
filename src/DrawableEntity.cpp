//
// Created by jacks on 12/06/2025.
//

#include "DrawableEntity.h"


DrawableEntity::DrawableEntity() {
    renderInfo = RenderInfo(RenderInfo::RECT, SDL_Color{255, 255, 255, 255}, SDL_Point{50, 50}, SDL_Point{25, 25});
}

DrawableEntity::DrawableEntity(const RenderInfo &render_info) {
    renderInfo = render_info;
}

RenderInfo &DrawableEntity::getRenderInfo() {
    return renderInfo;
}
