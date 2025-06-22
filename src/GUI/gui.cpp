#include <iostream>

#include "GUI/gui.h"
#include "GUI/DrawableEntity.h"
#include "GUI/VisualBoard.h"

ChessGui::ChessGui(){
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    visualBoard = new VisualBoard(Vec2D(800, 800), this);
    registerEntity(visualBoard);

    SDL_Init(SDL_INIT_VIDEO);
}

ChessGui::ChessGui(EngineBase* engine) : engine_(engine){
    window = SDL_CreateWindow("Chess", 800, 800, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    visualBoard = new VisualBoard(Vec2D(800, 800), this);
    registerEntity(visualBoard);

    SDL_Init(SDL_INIT_VIDEO);
}

bool ChessGui::wasInit() const{ return window != nullptr && renderer != nullptr; }

EngineBase *ChessGui::getEngine() const{ return engine_; }
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
                boardManager_.undoMove();
            break;

        default:
            break;
    }
}

void ChessGui::handleKeyUp(const SDL_Keycode key){
    for (std::pair<SDL_Keycode, bool> keycode: modifiersSet) {
        if (keycode.first == key) {
            modifiersSet[keycode.first] = false;
        }
    }
}

void ChessGui::handleMouseDown(const Uint8 button){
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
    const int file = 1 + static_cast<int>(x / (visualBoard->boardSize().x / 8.f));
    const int rank = 1 + static_cast<int>(8 - (y / (visualBoard->boardSize().y / 8.f)));
    const char clickedFile = 'a' + file - 1;

    int candidateClickedSquare = rankAndFileToSquare(rank, file);

    if (boardManager_.getBitboards()->getPiece(rank, file).has_value()) { clickedSquare = candidateClickedSquare; }
    std::cout << "Clicked on:" << clickedFile << rank << "Square: " << candidateClickedSquare << std::endl;
}

void ChessGui::addMouseRelease(const int x, const int y){
    // need to effectively round down to the nearest rank/file
    const int rank = 1 + static_cast<int>(8 - (y / (visualBoard->boardSize().y / 8.f)));
    const int file = 1 + static_cast<int>(x / (visualBoard->boardSize().x / 8.f));

    if (clickedSquare == -1) { return; }

    int clickedRank, clickedFile;
    squareToRankAndFile(clickedSquare, clickedRank, clickedFile);

    auto candidateMovePiece = boardManager_.getBitboards()->getPiece(clickedRank, clickedFile);

    if (!candidateMovePiece.has_value()) { return; }

    auto move = Move{
                .piece = candidateMovePiece.value(),
                .rankFrom = clickedRank,
                .fileFrom = clickedFile,
                .rankTo = rank, .fileTo = file
            };

    std::cout << "Move: " << move.toUCI() << std::endl;

    if (!boardManager_.tryMove(move)) {
        clickedSquare = -1;
        return;
    }

    std::cout << move.result << std::endl;
    clickedSquare = -1;
}
