#include "server/client_handler/controllers/game_controller.h"

#include "server/session/logic/GameSessionFacade.h"

GameController::GameController(GameSessionFacade& game,
                               const PlayerId client_id, Api& api,
                               Receiver& receiver, IGameEvents& handler,
                               spdlog::logger* log)
    : log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler),
      game(game) {
    // TODO(juli): borrar, para que el lint no moleste
    (void)this->api;
    (void)this->client_id;
    (void)this->log;
    (void)this->dispatcher;
    log->debug("controlling game");
    Receiver::Listener::subscribe(receiver);
    GameSessionFacade::Listener::subscribe(game);
}
GameController::~GameController() {
    Receiver::Listener::unsubscribe();
    GameSessionFacade::Listener::unsubscribe();
}

void GameController::on_turn_left() { game.startTurningLeft(client_id); }
void GameController::on_stop_turning_left() { game.stopTurningLeft(client_id); }
void GameController::on_turn_right() { game.startTurningRight(client_id); }
void GameController::on_stop_turning_right() {
    game.stopTurningRight(client_id);
}
void GameController::on_accelerate() { game.startAccelerating(client_id); }
void GameController::on_stop_accelerating() {
    game.stopAccelerating(client_id);
}
void GameController::on_reverse() { game.startReversing(client_id); }
void GameController::on_stop_reversing() { game.stopReversing(client_id); }
void GameController::on_nitro() { game.useNitro(client_id); }

void GameController::on_snapshot(const WorldSnapshot& snapshot) {
    api.send_game_snapshot(snapshot.raceTimeLeft, snapshot.players);
}
