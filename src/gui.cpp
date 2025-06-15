#include "gui.h"

#include <iostream>

#include "DrawableEntity.h"


ChessGui::ChessGui() {
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;

    SDL_Init(SDL_INIT_VIDEO);
}

bool ChessGui::wasInit() const {
    return window != nullptr && renderer != nullptr;
}


void ChessGui::loop() {
    if (!wasInit()) {
        return;
    }

    while (running) {
        pollEvents();
        render();
        SDL_Delay(16); // Simulate ~60 FPS
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ChessGui::render() const {
    // draw black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // do the entities
    for (const auto &drawable: drawables) {
        drawable->draw(renderer);
    }

    // flip it
    SDL_RenderPresent(renderer);
}

void ChessGui::registerEntity(DrawableEntity *entity) {
    drawables.push_back(entity);
}

SDL_Renderer *ChessGui::getRenderer() const {
    return renderer;
}

void ChessGui::handleMouseDown(const Uint8 button) {
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);

        }
        default: ;
    }
}

void ChessGui::pollEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                handleKeypress(event.key.key);

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                handleMouseDown(event.button.button);

            default:
                break;
        }
    }
}

void ChessGui::handleKeypress(const SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_ESCAPE:
            running = false;
            break;

        case SDLK_SPACE:
            break;

        default:
            break;
    }
}
