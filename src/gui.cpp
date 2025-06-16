#include "gui.h"

#include <iostream>

#include "BoardGUI.h"
#include "DrawableEntity.h"


ChessGui::ChessGui() {
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    board_background_ = new BoardGUI(Vec2D(800, 800), this);
    registerEntity(board_background_);

    SDL_Init(SDL_INIT_VIDEO);
}

bool ChessGui::wasInit() {
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

void ChessGui::render() {
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

SDL_Renderer *ChessGui::getRenderer() {
    return renderer;
}

void ChessGui::handleMouseDown(const Uint8 button) {
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            const int file = static_cast<int>(x / (board_background_->boardSize.x / 8.f));
            const int rank = static_cast<int>(y / (board_background_->boardSize.y / 8.f));
            heldPiece = board_background_->pieceAtLocation(rank, file);



        }
        default:
            break;
    }
}

void ChessGui::handleMouseUp(const Uint8 button) {
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            const Vec2D squareSize = board_background_->boardSize / 8.f;
            const int file = static_cast<int>(x / (board_background_->boardSize.x / 8.f));
            const int rank = static_cast<int>(y / (board_background_->boardSize.y / 8.f));
            const Vec2D newLocation = {
                .x = file * squareSize.x,
                .y = rank * squareSize.y
            };

            if (heldPiece) {

                heldPiece->setLocation(newLocation); // Modify the shared_ptr's contents directly
            }
            break;
        }
        default:
            break;
    }
}

void ChessGui::pollEvents() {
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_EVENT_QUIT:
            running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            handleKeypress(event.key.key);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            handleMouseDown(event.button.button);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:

            handleMouseUp(event.button.button);
            break;

        default:
            break;
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
