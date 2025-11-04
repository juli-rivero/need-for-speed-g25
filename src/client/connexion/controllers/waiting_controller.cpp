#include "client/connexion/controllers/waiting_controller.h"

#include "spdlog/spdlog.h"

WaitingController::WaitingController(WaitingWindow& waiting_window)
    : waiting_window(waiting_window) {
    // TODO(fran): borrar
    (void)this->waiting_window;
    spdlog::trace("controlling lobby");
}
void WaitingController::recv(const dto_lobby::StartResponse&) {
    throw std::runtime_error("not implemented");
}
