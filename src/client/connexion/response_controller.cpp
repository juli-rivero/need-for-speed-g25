#include "client/connexion/response_controller.h"

ResponseController::ResponseController()
    : browser_controller(nullptr),
      lobby_controller(nullptr),
      game_controller(nullptr) {}

void ResponseController::control(Browser& browser) {
    if (browser_controller) delete browser_controller;
    browser_controller = new BrowserController(browser);
}

void ResponseController::decontrol(Browser&) {
    if (browser_controller) delete browser_controller;
    browser_controller = nullptr;
}

void ResponseController::control(Lobby& lobby) {
    if (lobby_controller) delete lobby_controller;
    lobby_controller = new LobbyController(lobby);
}

void ResponseController::decontrol(Lobby&) {
    if (lobby_controller) delete lobby_controller;
    lobby_controller = nullptr;
}

void ResponseController::control(Game& game) {
    if (game_controller) delete game_controller;
    game_controller = new GameController(game);
}

void ResponseController::decontrol(Game&) {
    if (game_controller) delete game_controller;
    game_controller = nullptr;
}

ResponseController::~ResponseController() {
    delete browser_controller;
    delete lobby_controller;
    delete game_controller;
}

void ResponseController::on(const dto_session::LeaveResponse&) {}
void ResponseController::on(const dto_session::JoinResponse&) {}
void ResponseController::on(const dto_session::SearchResponse&) {}
void ResponseController::on(const dto_lobby::StartResponse&) {}
void ResponseController::on(const dto::ErrorResponse&) {}
