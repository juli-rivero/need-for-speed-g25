#include "client/connexion/lobby_controller.h"

LobbyController::LobbyController(Lobby& lobby) : lobby(lobby) {
    // TODO(fran): borrar
    (void)this->lobby;
}
void LobbyController::on(const dto_lobby::StartResponse&) {
    throw std::runtime_error("not implemented");
}
