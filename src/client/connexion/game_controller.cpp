#include "client/connexion/game_controller.h"

#include "spdlog/spdlog.h"

GameController::GameController(Game& game) : game(game) {
    // TODO(fran): borrar
    (void)this->game;
    spdlog::debug("controlling game");
}
