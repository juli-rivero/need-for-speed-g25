#include "client/connexion/controllers/searching_controller.h"

#include "client/qt/windows/searching_window.h"
#include "spdlog/spdlog.h"

using dto::ErrorResponse;
using dto_search::JoinResponse;
using dto_search::SearchResponse;

SearchingController::SearchingController(SearchingWindow& searching_window)
    : searching_window(searching_window) {
    // TODO(fran): borrar
    spdlog::trace("controlling browser");
}
void SearchingController::recv(const JoinResponse& r) {
    searching_window.joinGame(r.session_id);
}
void SearchingController::recv(const SearchResponse& r) {
    searching_window.updateGamesList(r.sessions);
}
void SearchingController::recv(const ErrorResponse&) {
    throw std::runtime_error("not implemented");
}
