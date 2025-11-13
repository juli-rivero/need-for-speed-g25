#include "server/client_handler/controllers/game_controller.h"

#include "server/session/logic/GameSessionFacade.h"

GameController::GameController(GameSessionFacade&, const int client_id,
                               Api& api, Receiver& receiver,
                               IGameEvents& handler, spdlog::logger* log)
    : Receiver::Listener(receiver),
      log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler) {
    // TODO(juli): borrar, para que el lint no moleste
    (void)this->api;
    (void)this->client_id;
    (void)this->log;
    (void)this->dispatcher;
    log->debug("controlling game");
}
