#include "server/session/game.h"

GameListener::GameListener(Game& game) : Listener(game.emitter), game(game) {}

Game::Game(const std::unordered_map<int, UserSetup>& users_setup)
    : users_setup(users_setup) {
    // TODO(elvis): borrar
    (void)this->users_setup;
    (void)this->game_loop;
}
