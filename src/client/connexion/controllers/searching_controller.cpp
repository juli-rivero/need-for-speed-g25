#include "client/connexion/controllers/searching_controller.h"

#include "client/qt/windows/searching_window.h"
#include "spdlog/spdlog.h"

SearchingController::SearchingController(SearchingWindow& searching_window)
    : searching_window(searching_window) {
    // TODO(fran): borrar
    spdlog::trace("controlling browser");
}
void SearchingController::recv(const dto_session::JoinResponse&) {
    throw std::runtime_error("not implemented");
}
void SearchingController::recv(const dto_session::SearchResponse& r) {
    searching_window.updateGamesList(r.sessions);
}
void SearchingController::recv(const dto::ErrorResponse&) {
    throw std::runtime_error("not implemented");
}
