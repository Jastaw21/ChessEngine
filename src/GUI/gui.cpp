#include <iostream>

#include "GUI/VisualBoard.h"
#include "GUI/DrawableEntity.h"
#include "GUI/gui.h"

ChessGui::ChessGui(){
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    board_background_ = new VisualBoard(Vec2D(800, 800), this);
    registerEntity(board_background_);

    SDL_Init(SDL_INIT_VIDEO);
}

ChessGui::ChessGui(EngineBase *engine) : engine_(engine){
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    board_background_ = new VisualBoard(Vec2D(800, 800), this);
    registerEntity(board_background_);

    SDL_Init(SDL_INIT_VIDEO);
}

bool ChessGui::wasInit() const{ return window != nullptr && renderer != nullptr; }


void ChessGui::loop(){
    if (!wasInit()) { return; }

    while (running) {
        pollEvents();
        render();
        SDL_Delay(16); // Simulate ~60 FPS
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ChessGui::render(){
    // draw black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // do the entities
    for (const auto &drawable: drawables) { drawable->draw(renderer); }

    // flip it
    SDL_RenderPresent(renderer);
}

void ChessGui::registerEntity(DrawableEntity *entity){ drawables.push_back(entity); }

EngineBase *ChessGui::getEngine() const{ return engine_; }


SDL_Renderer *ChessGui::getRenderer() const{ return renderer; }

void ChessGui::handleMouseDown(const Uint8 button){
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            const int file = static_cast<int>(x / (board_background_->boardSize().x / 8.f));
            const int rank = 1 + static_cast<int>(8 - (y / (board_background_->boardSize().y / 8.f)));
            const char clickedFile = 'a' + file;
            std::cout << "Clicked on:" << clickedFile << rank << std::endl;

            heldPiece = board_background_->pieceAtLocation(rank, file);
            if (heldPiece != nullptr) {
                const char pieceFile = 'a' + heldPiece->getFile();
                const auto pieceType = pieceNames.at(heldPiece->getPiece());
                std::cout << "Piece at: " << pieceFile << rank << "is: " << pieceType << std::endl;
            }
        }
        default:
            break;
    }
}

void ChessGui::handleMouseUp(const Uint8 button){
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);

            // need to effectively round down to the nearest rank/file
            const int file = static_cast<int>(x / (board_background_->boardSize().x / 8.f));
            const int rank = 1 + static_cast<int>(8 - (y / (board_background_->boardSize().y / 8.f)));

            if (!heldPiece) { return; }

            const auto move = Move{
                        .piece = heldPiece->getPiece(),
                        .rankFrom =heldPiece->getRank(),
                        .fileFrom = heldPiece->getFile(),
                        .rankTo = rank, .fileTo = file
                    };

            if (!boardManager_.checkMove(move)) {
                heldPiece.reset();
                return;
            }

           makeMove(move);
            heldPiece.reset();

            break;
        }
        default:
            break;
    }
}

void ChessGui::pollEvents(){
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

void ChessGui::handleKeypress(const SDL_Keycode keycode){
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

void ChessGui::makeMove(const Move &move){

    heldPiece->setLocation(move.rankTo, move.fileTo);
    boardManager_.makeMove(move);

}
