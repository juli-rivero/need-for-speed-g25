#include "server/client_handler/controllers/game_controller.h"

#include "../../config/convertor.h"
#include "server/session/logic/GameSessionFacade.h"

GameController::GameController(GameSessionFacade& game,
                               const PlayerId client_id, Api& api,
                               Receiver& receiver, IGameEvents& handler,
                               const YamlGameConfig& cfg, spdlog::logger* log)
    : log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler),
      game(game),
      cfg(cfg) {
    log->debug("controlling game");
    Receiver::Listener::subscribe(receiver);
    GameSessionFacade::Listener::subscribe(game);

    // TODO(juli): usar
    (void)this->log;
    (void)this->dispatcher;
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

void GameController::on_snapshot(const GameSnapshot& snapshot) {
    GameSnapshot transformed_snapshot = snapshot;

    const Convertor convertor(cfg.getPixelsPerMeter());

    for (auto& player : transformed_snapshot.players) {
        player.car.angle = convertor.toDegrees(player.car.angle);
        player.car.bound = convertor.toPixels(player.car.bound);
    }
    for (auto& npc : transformed_snapshot.npcs) {
        npc.angle = convertor.toDegrees(npc.angle);
        npc.bound = convertor.toPixels(npc.bound);
    }

    api.send_game_snapshot(transformed_snapshot);
}
void GameController::on_collision_event(const CollisionEvent& event) {
    api.send_collision_event(event);
}
void GameController::on_cheat(Cheat cheat) { game.cheat(client_id, cheat); }
void GameController::on_upgrade_request(UpgradeStat upgrade_stat) {
    game.upgrade(client_id, upgrade_stat);
}
