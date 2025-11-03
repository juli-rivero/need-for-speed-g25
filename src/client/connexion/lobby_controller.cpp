#include "client/connexion/lobby_controller.h"

#include "spdlog/spdlog.h"

LobbyController::LobbyController(WaitingRoomWidget& waiting_room)
    : waiting_room(waiting_room) {
    // TODO(fran): borrar
    (void)this->waiting_room;
    spdlog::trace("controlling lobby");
}
void LobbyController::on(const dto_lobby::StartResponse&) {
    throw std::runtime_error("not implemented");
}
