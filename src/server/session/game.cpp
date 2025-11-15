#include "server/session/game.h"

#include "common/thread.h"
#include "logic/GameSessionFacade.h"
#include "spdlog/logger.h"

Game::Game(const std::unordered_map<int, UserSetup>& users_setup,
           spdlog::logger* log)
    : log(log), users_setup(users_setup) {
    // TODO(elvis): borrar
    (void)this->users_setup;
    (void)this->log;
    log->debug("Game started");
}

Game::Listener::Listener(Game& game)
    : common::Listener<Game::Listener>(game.emitter), game(game) {}
