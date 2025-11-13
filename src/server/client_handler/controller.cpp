#include "server/client_handler/controller.h"

Controller::Controller(SessionsMonitor& monitor, const PlayerId id, Api& api,
                       Receiver& receiver, spdlog::logger* log)
    : log(log),
      id(id),
      api(api),
      receiver(receiver),
      search_controller(std::make_unique<SearchController>(
          monitor, id, api, receiver, *this, log)),
      session_controller(nullptr),
      game_controller(nullptr) {}

void Controller::run() {
    while (should_keep_running()) {
        try {
            events.pop()();
        } catch (ClosedQueue&) {}
    }
}
void Controller::stop() {
    Thread::stop();
    events.close();
}

void Controller::on_join_session(Session& session) {
    events.try_push([&session, this] {
        session_controller = std::make_unique<SessionController>(
            session, id, api, receiver, *this, log);
    });
}

void Controller::on_start_game(GameSessionFacade& game) {
    events.try_push([&game, this] {
        game_controller = std::make_unique<GameController>(
            game, id, api, receiver, *this, log);
    });
}

void Controller::on_game_end() {
    events.try_push([this] {
        session_controller = nullptr;
        game_controller = nullptr;
    });
}
