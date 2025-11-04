#include "server/client_handler/controller.h"

Controller::Controller(SessionsMonitor& monitor, const int id, Sender& sender,
                       spdlog::logger* log)
    : log(log),
      id(id),
      sender(sender),
      search_controller(new SearchController(monitor, id, sender, *this, log)),
      session_controller(nullptr),
      game_controller(nullptr) {}

void Controller::on(const dto_search::JoinRequest& r) {
    std::lock_guard lock(mtx);
    if (search_controller) search_controller->on(r);
}

void Controller::on(const dto_search::SearchRequest& r) {
    std::lock_guard lock(mtx);
    if (search_controller) search_controller->on(r);
}

void Controller::on(const dto_session::LeaveRequest& r) {
    std::lock_guard lock(mtx);
    if (session_controller) session_controller->on(r);
}

void Controller::on(const dto_session::StartRequest& r) {
    std::lock_guard lock(mtx);
    if (session_controller) session_controller->on(r);
}

Controller::~Controller() { decontrol_all(); }

void Controller::decontrol_all() {
    std::lock_guard lock(mtx);
    delete search_controller;
    delete session_controller;
    delete game_controller;
}

void Controller::on_join_session(Session& s) {
    std::lock_guard lock(mtx);
    if (session_controller) delete session_controller;
    session_controller = new SessionController(s, id, sender, *this, log);
}

void Controller::on_start_game(Game& game) {
    std::lock_guard lock(mtx);
    if (game_controller) delete game_controller;
    game_controller = new GameController(game, id, sender, *this, log);
}

void Controller::on_game_end() {
    std::lock_guard lock(mtx);
    if (session_controller) delete session_controller;
    session_controller = nullptr;
    if (game_controller) delete game_controller;
    game_controller = nullptr;
}
