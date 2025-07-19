#include <iostream>

#include "GUI/gui.h"
#include "ChessPlayer.h"
#include "Engine/EngineBase.h"

#include "GUI/DrawableEntity.h"
#include "GUI/VisualBoard.h"


ChessGui::ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer){
    initSDLStuff();
    matchManager_ = new MatchManager(whitePlayer, blackPlayer);
}

void ChessGui::initSDLStuff(){
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    visualBoard = new VisualBoard(Vec2D(800, 800), this);
    registerEntity(visualBoard);
    SDL_Init(SDL_INIT_VIDEO);
}

ChessGui::ChessGui(){
    initSDLStuff();
    matchManager_ = new MatchManager();
}

bool ChessGui::wasInit() const{ return window != nullptr && renderer != nullptr; }

SDL_Renderer *ChessGui::getRenderer() const{ return renderer; }

void ChessGui::registerEntity(DrawableEntity* entity){ drawables.push_back(entity); }


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

void ChessGui::render() const{
    // draw black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // do the entities
    for (const auto& drawable: drawables) { drawable->draw(renderer); }

    // flip it
    SDL_RenderPresent(renderer);
}

void ChessGui::pollEvents(){
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_EVENT_QUIT:
            running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            handleKeyDown(event.key.key);
            break;

        case SDL_EVENT_KEY_UP:
            handleKeyUp(event.key.key);
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

void ChessGui::handleKeyDown(const SDL_Keycode keycode){
    switch (keycode) {
        case SDLK_ESCAPE:
            running = false;
            break;

        case SDLK_SPACE:
            break;

        case SDLK_LALT:
        case SDLK_LSHIFT:
        case SDLK_LCTRL:
            modifiersSet[keycode] = true;
            break;
        case SDLK_Z:
            if (modifiersSet[SDLK_LCTRL])
                matchManager_->getBoardManager().undoMove();
            break;
        default:
            break;
    }
}

void ChessGui::handleKeyUp(const SDL_Keycode key){
    for (std::pair<SDL_Keycode, bool> keycode: modifiersSet) {
        if (keycode.first == key) { modifiersSet[keycode.first] = false; }
    }
}

void ChessGui::handleMouseDown(const Uint8 button){
    if (matchManager_->currentPlayer->playerType != HUMAN) { return; }

    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            addMouseClick(x, y);
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
            addMouseRelease(x, y);
            break;
        }
        default:
            break;
    }
}

void ChessGui::addMouseClick(const int x, const int y){
    // where on the screen did we click?
    const int file = 1 + static_cast<int>(x / (visualBoard->boardSize().x / 8.f));
    const int rank = 1 + static_cast<int>(8 - y / (visualBoard->boardSize().y / 8.f));
    if (file < 1 || file > 8 || rank < 1 || rank > 8) { return; }
    const int candidateClickedSquare = rankAndFileToSquare(rank, file);

    // check if we clicked on a piece
    const auto clickedPiece = matchManager_->getBoardManager().getBitboards()->getPiece(rank, file);

    if (clickedPiece.has_value()) {
        clickedSquare = candidateClickedSquare;
        if (matchManager_->currentPlayer->playerType == HUMAN) {
            const auto humanPlayer = static_cast<HumanPlayer *>(matchManager_->currentPlayer);
            humanPlayer->pickUpPiece(candidateClickedSquare);
        }
    }
}


void ChessGui::addMouseRelease(const int x, const int y){
    // need to effectively round down to the nearest rank/file
    const int rank = 1 + static_cast<int>(8 - y / (visualBoard->boardSize().y / 8.f));
    const int file = 1 + static_cast<int>(x / (visualBoard->boardSize().x / 8.f));

    if (matchManager_->currentPlayer->playerType != HUMAN) { return; }

    auto humanPlayer = static_cast<HumanPlayer *>(matchManager_->currentPlayer);

    if (humanPlayer->getHeldPiece() == -1)
        return;
    auto move = humanPlayer->selectDestination(rankAndFileToSquare(rank, file), &matchManager_->getBoardManager());
    matchManager_->parseUCI("bestmove " + move.toUCI());
}

