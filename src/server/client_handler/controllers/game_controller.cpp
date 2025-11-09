#include "server/client_handler/controllers/game_controller.h"

GameController::GameController(Game& game, const int client_id, Api& api,
                               Receiver& receiver, IGameEvents& handler,
                               spdlog::logger* log)
    : Game::Listener(game),
      Receiver::Listener(receiver),
      log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler) {
    // TODO(juli): borrar, para que el lint no moleste
    (void)this->api;
    (void)this->client_id;
    (void)this->log;
    log->debug("controlling game");
}
void GameController::on_end_game() { dispatcher.on_game_end(); }
void GameController::on_snapshot() {}
