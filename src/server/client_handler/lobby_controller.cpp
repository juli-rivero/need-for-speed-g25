#include "server/client_handler/lobby_controller.h"

LobbyController::LobbyController(Session& session, const int client_id,
                                 Sender& sender, ILobbyEvents& handler)
    : SessionListener(session),
      client_id(client_id),
      sender(sender),
      dispatcher(handler) {}

void LobbyController::on(const dto_lobby::StartRequest& start_request) {
    try {
        session.set_ready(client_id, start_request.ready);
    } catch (std::runtime_error& e) {
        sender.send(dto::ErrorResponse{e.what()});
    }
}

void LobbyController::on_start_game(Game& game) {
    dispatcher.on_start_game(game);
}
