#include "server/client_handler/controllers/game_controller.h"

GameController::GameController(Game& game, const int client_id, Sender& sender,
                               IGameEvents& handler, spdlog::logger* log)
    : GameListener(game),
      log(log),
      client_id(client_id),
      sender(sender),
      dispatcher(handler) {
    // TODO(juli): borrar, para que el lint no moleste
    (void)this->sender;
    (void)this->client_id;
    (void)this->log;
    log->debug("controlling game");
}
void GameController::on_end_game() { dispatcher.on_game_end(); }
void GameController::on_snapshot() {}
