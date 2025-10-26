#include "server/client_handler/controller.h"

Controller::Controller(SessionsMonitor& monitor, const int id, Sender& sender)
    : id(id),
      sender(sender),
      browser_controller(new BrowserController(monitor, id, sender, *this)),
      lobby_controller(nullptr),
      game_controller(nullptr) {}

void Controller::on(const dto_session::LeaveRequest& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}

void Controller::on(const dto_session::JoinRequest& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}

void Controller::on(const dto_session::SearchRequest& r) {
    std::lock_guard lock(mtx);
    if (browser_controller) browser_controller->on(r);
}

void Controller::on(const dto_lobby::StartRequest& r) {
    std::lock_guard lock(mtx);
    if (lobby_controller) lobby_controller->on(r);
}

Controller::~Controller() { decontrol_all(); }

void Controller::decontrol_all() {
    std::lock_guard lock(mtx);
    delete browser_controller;
    delete lobby_controller;
    delete game_controller;
}

void Controller::on_join_session(Session& s) {
    std::lock_guard lock(mtx);
    if (lobby_controller) delete lobby_controller;
    lobby_controller = new LobbyController(s, id, sender, *this);
}

void Controller::on_start_game(Game& game) {
    std::lock_guard lock(mtx);
    if (game_controller) delete game_controller;
    game_controller = new GameController(game, id, sender, *this);
}

void Controller::on_game_end() {
    std::lock_guard lock(mtx);
    if (lobby_controller) delete lobby_controller;
    lobby_controller = nullptr;
    if (game_controller) delete game_controller;
    game_controller = nullptr;
}
