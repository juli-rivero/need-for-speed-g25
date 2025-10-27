#include "client/connexion/lobby_controller.h"

#include "spdlog/spdlog.h"

LobbyController::LobbyController(Lobby& lobby) : lobby(lobby) {
    // TODO(fran): borrar
    (void)this->lobby;
    spdlog::trace("controlling lobby");
}
void LobbyController::on(const dto_lobby::StartResponse&) {
    throw std::runtime_error("not implemented");
}
