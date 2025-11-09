#include "server/client_handler/controller.h"

Controller::Controller(SessionsMonitor& monitor, const int id, Api& api,
                       Receiver& receiver, spdlog::logger* log)
    : log(log),
      id(id),
      api(api),
      receiver(receiver),
      search_controller(std::make_unique<SearchController>(
          monitor, id, api, receiver, *this, log)),
      session_controller(nullptr),
      game_controller(nullptr) {}

void Controller::on_join_session(Session& s) {
    session_controller =
        std::make_unique<SessionController>(s, id, api, receiver, *this, log);
}

void Controller::on_start_game(Game& game) {
    game_controller =
        std::make_unique<GameController>(game, id, api, receiver, *this, log);
}

void Controller::on_game_end() {
    session_controller = nullptr;
    game_controller = nullptr;
}
