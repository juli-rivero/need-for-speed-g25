#include "lobby_controller.h"

LobbyController::LobbyController(Session& session, const int client_id,
                                 Sender& sender, ILobbyEvents& handler,
                                 spdlog::logger* log)
    : SessionListener(session),
      log(log),
      client_id(client_id),
      sender(sender),
      dispatcher(handler) {
    log->debug("controlling lobby");
}

void LobbyController::on(const dto_lobby::StartRequest& start_request) {
    try {
        session.set_ready(client_id, start_request.ready);
        log->trace(start_request.ready ? "ready" : "not ready");
    } catch (std::runtime_error& e) {
        log->trace("could not set {}: {}",
                   start_request.ready ? "ready" : "not ready", e.what());
        sender.send(dto::ErrorResponse{e.what()});
    }
}

void LobbyController::on_start_game(Game& game) {
    dispatcher.on_start_game(game);
}
