#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>

#include "GUI/gui.h"
#include "ChessPlayer.h"
#include "HumanPlayer.h"
#include "Engine/EngineBase.h"

#include "GUI/DrawableEntity.h"
#include "GUI/VisualBoard.h"

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"


constexpr int deltaTime = 16;


Sound buildAudioStream(const std::string& path){
    Sound sound;
    mp3dec_t mp3dec;
    mp3dec_file_info_t info;
    mp3dec_load(&mp3dec, path.c_str(),
                &info,
                nullptr, nullptr);

    std::vector<float> float_samples(info.samples);
    for (size_t i = 0; i < info.samples; i++) {
        float_samples[i] = info.buffer[i] / 32768.0f; // Convert int16 to float
    }

    free(info.buffer);

    SDL_AudioSpec sourceSpec;
    sourceSpec.format = SDL_AUDIO_F32;
    sourceSpec.channels = info.channels;
    sourceSpec.freq = info.hz;

    sound.spec = sourceSpec;
    sound.data = float_samples;

    return sound;
}


ChessGui::ChessGui(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer){
    initSDLStuff();
    initChessStuff(whitePlayer, blackPlayer);
}

void ChessGui::initGuiStuff(){
    visualBoard = std::make_shared<VisualBoard>(Vec2D{800, 800}, this, Vec2D{0, 20});
    registerEntity(visualBoard);

    const auto fontPath = std::string(RESOURCE_DIR) + "/fonts/LatoWeb-Bold.ttf";
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), 8);
    if (!font) {
        printf("TTF_OpenFont failed: %s\n", SDL_GetError());
        // Handle error
    }
    evaluationBar_ = std::make_shared<EvaluationBar>(Vec2D{0, 0}, Vec2D{800, 20}, font);
    registerEntity(evaluationBar_);

    picker_ = std::make_shared<PromotionPiecePicker>(visualBoard.get());
    registerEntity(picker_);
}

void ChessGui::initSDLStuff(){
    window = SDL_CreateWindow("Chess", 800, 900, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    running = true;
    TTF_Init();
    initGuiStuff();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    spec.freq = 44100;
    audioID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    movePiecesSound = buildAudioStream(std::string(RESOURCE_DIR) + "/sounds/move-self.mp3");
    captureSound = buildAudioStream(std::string(RESOURCE_DIR) + "/sounds/capture.mp3");
    illegalSound = buildAudioStream(std::string(RESOURCE_DIR) + "/sounds/illegal.mp3");
    clickSound = buildAudioStream(std::string(RESOURCE_DIR) + "/sounds/click.mp3");
}

void ChessGui::initChessStuff(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer){
    matchManager_ = std::make_shared<MatchManager>(whitePlayer, blackPlayer);
    matchManager_->setGUI(this);
    evaluator_ = std::make_shared<Evaluator>(&matchManager_.get()->getBoardManager());
}

ChessGui::ChessGui(){
    initSDLStuff();
    initChessStuff(nullptr, nullptr);
}

bool ChessGui::wasInit() const{ return window != nullptr && renderer != nullptr; }

SDL_Renderer* ChessGui::getRenderer() const{ return renderer; }

void ChessGui::registerEntity(const std::shared_ptr<DrawableEntity>& entity){ drawables.push_back(entity); }

void ChessGui::updateGame(const int deltaTime_){
    if (!gameStarted) {
        matchManager_->startGame();
        gameStarted = true;
    }

    runningTime += deltaTime_;
    if (runningTime >= 100) {
        matchManager_->tick();
        runningTime = 0;
    }

    if (bMouseHeld) {
        float mouseX;
        float mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        visualBoard->updateHeldPieceLocation(Vec2D{mouseX, mouseY});
    }

    if (outboundMoveIsReady) { completeMove(); }

    if (bEvaluationDirty) { updateEvaluationBar(); }
}

void ChessGui::loop(){
    if (!wasInit()) { return; }

    while (running) {
        pollEvents();
        render();
        SDL_Delay(deltaTime); // Simulate ~60 FPS
        updateGame(deltaTime);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ChessGui::addMouseClick(const int x, const int y){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }
    if (picker_->isBeingDrawn()) { return; }

    visualBoard->clearHighlights();
    // where on the screen did we click?
    const auto rankAndFile = getRankAndFile(x, y);
    if (rankAndFile.file < 1 || rankAndFile.file > 8 || rankAndFile.rank < 1 || rankAndFile.rank > 8) { return; }
    const int candidateClickedSquare = rankAndFileToSquare(rankAndFile.rank, rankAndFile.file);

    // check if we clicked on a piece
    const auto clickedPiece = matchManager_->getBoardManager().getBitboards()->getPiece(
        rankAndFile.rank, rankAndFile.file);

    if (!clickedPiece.has_value()) { return; } // no piece clicked
    // wrong colour clicked
    if (pieceColours[clickedPiece.value()] != matchManager_->getBoardManager().getCurrentTurn()) { return; }

    clickedSquare = candidateClickedSquare;
    visualBoard->highlightSquare(rankAndFile, HighlightType::RECENT_MOVE);
    visualBoard->pickUpPiece(rankAndFile, clickedPiece.value());
    playSound(clickSound);
}

void ChessGui::handleInvalidMove(){
    visualBoard->clearHighlights();
    visualBoard->highlightSquare({outboundMove.rankTo, outboundMove.fileTo}, HighlightType::INVALID_MOVE);
    clickedSquare = -1;
    playSound(illegalSound);
    outboundMoveIsReady = false;
    outboundMove = Move();
    visualBoard->dropPiece();
}

void ChessGui::updateEvaluationBar(){
    auto eval = evaluator_->evaluate();
    if (matchManager_->getBoardManager().getCurrentTurn() == BLACK) {
        eval *= -1.f; // invert it, if it's good for black, we turn it negative.
    }

    const auto resultEval = MathUtility::map(eval, -600, 600, 0.f, 1.f);
    evaluationBar_->set_evaluation(resultEval);

    bEvaluationDirty = false;
}

void ChessGui::completeMove(){
    if (!matchManager_->getBoardManager().checkMove(outboundMove)) {
        handleInvalidMove();
        return;
    }
    const auto humanPlayer = static_cast<HumanPlayer*>(matchManager_->currentPlayer());
    humanPlayer->addMessage("bestmove " + outboundMove.toUCI());
    visualBoard->highlightSquare({outboundMove.rankTo, outboundMove.fileTo}, HighlightType::RECENT_MOVE);
    visualBoard->dropPiece();
    outboundMove = Move();
    outboundMoveIsReady = false;
    bEvaluationDirty = true;
}

void ChessGui::beginMove(const RankAndFile rankAndFile, const Piece candidatePiece){
    outboundMove = Move(candidatePiece, clickedSquare,
                        rankAndFileToSquare(rankAndFile.rank, rankAndFile.file));

    // if it's a promotion move, need to wait for further input
    if (
        (candidatePiece == WP && rankAndFile.rank == 8)
        || (candidatePiece == BP && rankAndFile.rank == 1)
    ) {
        picker_->setShouldDraw(true, matchManager_->getBoardManager().getCurrentTurn());

        // not ready, but set the prelim move
        outboundMoveIsReady = false;
    } else { outboundMoveIsReady = true; }
}

void ChessGui::addMouseRelease(const int x, const int y){
    if (picker_->isBeingDrawn()) {
        const auto clickedPiece = picker_->getClickedPiece(x, y);
        if (clickedPiece == PIECE_N) { return; }
        outboundMove.promotedPiece = clickedPiece;
        outboundMoveIsReady = true;
        picker_->setShouldDraw(false);
    } else {
        // only allow moving if the human player can go
        if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }
        // if we were dragging a piece, drop it
        if (bMouseHeld) { visualBoard->dropPiece(); }

        const auto rankAndFile = getRankAndFile(x, y); // where did we click in rank and file format?
        if (clickedSquare == -1) { return; } // no piece selected when we originally clicked

        const auto candidateMovePiece = matchManager_->getBoardManager().getBitboards()->getPiece(clickedSquare);
        if (!candidateMovePiece.has_value()) { return; }
        beginMove(rankAndFile, candidateMovePiece.value());
    }
}

void ChessGui::receiveInfoOfEngineMove(const Move& move){
    visualBoard->clearHighlights();
    visualBoard->highlightSquare({move.rankFrom, move.fileFrom}, HighlightType::RECENT_MOVE);
    visualBoard->highlightSquare({move.rankTo, move.fileTo}, HighlightType::RECENT_MOVE);

    if (move.resultBits & MoveResult::CAPTURE) { playSound(captureSound); } else if (
        move.resultBits & MoveResult::PUSH) { playSound(movePiecesSound); }

    bEvaluationDirty = true;
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

void ChessGui::render() const{
    // draw black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // do the entities
    for (const auto& drawable: drawables) { drawable->draw(renderer); }

    // flip it
    SDL_RenderPresent(renderer);
}

void ChessGui::handleMouseDown(const Uint8 button){
    if (matchManager_->currentPlayer()->playerType != HUMAN) { return; }

    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x, y;
            SDL_GetMouseState(&x, &y);
            addMouseClick(x, y);
            bMouseHeld = true;
            break;
        }
        default:
            break;
    }
}

void ChessGui::handleMouseUp(const Uint8 button){
    switch (button) {
        case SDL_BUTTON_LEFT: {
            float x;
            float y;
            SDL_GetMouseState(&x, &y);
            addMouseRelease(x, y);
            bMouseHeld = false;
            break;
        }
        default:
            break;
    }
}

RankAndFile ChessGui::getRankAndFile(const int x, const int y) const{
    const int adjustedX = x - static_cast<int>(visualBoard->getParentOffset().x);
    const int adjustedY = y - static_cast<int>(visualBoard->getParentOffset().y);
    const int file = 1 + static_cast<int>(adjustedX / (visualBoard->boardSize().x / 8.f));
    const int rank = 1 + static_cast<int>(8 - adjustedY / (visualBoard->boardSize().y / 8.f));

    return {rank, file};
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

void ChessGui::playSound(const Sound& sound) const{
    const SDL_AudioSpec targetSpec = {
                .format = SDL_AUDIO_F32,
                .channels = 2,
                .freq = sound.spec.freq
            };

    SDL_AudioStream* stream = SDL_CreateAudioStream(&sound.spec, &targetSpec);
    SDL_BindAudioStream(audioID, stream);

    SDL_PutAudioStreamData(stream, sound.data.data(), sound.data.size() * sizeof(sound.data[0]));

    SDL_FlushAudioStream(stream);
}