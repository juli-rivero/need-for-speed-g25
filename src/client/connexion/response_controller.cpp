#include "client/connexion/response_controller.h"

#include "spdlog/spdlog.h"

ResponseController::ResponseController()
    : browser_controller(nullptr),
      lobby_controller(nullptr),
      game_controller(nullptr) {}

void ResponseController::control(LobbyWindow& searching_room) {
    std::lock_guard lock(mtx);
    delete browser_controller;
    browser_controller = new BrowserController(searching_room);
}

void ResponseController::decontrol(LobbyWindow&) {
    std::lock_guard lock(mtx);
    delete browser_controller;
    browser_controller = nullptr;
}

void ResponseController::control(WaitingRoomWidget& waiting_room) {
    std::lock_guard lock(mtx);
    delete lobby_controller;
    lobby_controller = new LobbyController(waiting_room);
}

void ResponseController::decontrol(WaitingRoomWidget&) {
    std::lock_guard lock(mtx);
    delete lobby_controller;
    lobby_controller = nullptr;
}

void ResponseController::control(Game& game) {
    std::lock_guard lock(mtx);
    delete game_controller;
    game_controller = new GameController(game);
}

void ResponseController::decontrol(Game&) {
    std::lock_guard lock(mtx);
    delete game_controller;
    game_controller = nullptr;
}

void ResponseController::decontrol_all() {
    std::lock_guard lock(mtx);
    delete browser_controller;
    browser_controller = nullptr;
    delete lobby_controller;
    lobby_controller = nullptr;
    delete game_controller;
    game_controller = nullptr;
}

ResponseController::~ResponseController() { decontrol_all(); }

void ResponseController::on(const dto_session::LeaveResponse& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}
void ResponseController::on(const dto_session::JoinResponse& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}
void ResponseController::on(const dto_session::SearchResponse& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}
void ResponseController::on(const dto_lobby::StartResponse& r) {
    std::lock_guard lock(mtx);
    if (lobby_controller) lobby_controller->on(r);
}
void ResponseController::on(const dto::ErrorResponse& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}
