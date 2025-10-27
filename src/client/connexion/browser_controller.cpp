#include "client/connexion/browser_controller.h"

#include "spdlog/spdlog.h"

BrowserController::BrowserController(Browser& browser) : browser(browser) {
    // TODO(fran): borrar
    (void)this->browser;
    spdlog::trace("controlling browser");
}
void BrowserController::on(const dto_session::LeaveResponse&) {
    throw std::runtime_error("not implemented");
}
void BrowserController::on(const dto_session::JoinResponse&) {
    throw std::runtime_error("not implemented");
}
void BrowserController::on(const dto_session::SearchResponse&) {
    throw std::runtime_error("not implemented");
}
void BrowserController::on(const dto::ErrorResponse&) {
    throw std::runtime_error("not implemented");
}
