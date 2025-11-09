#include "server/client_handler/controllers/session_controller.h"

using dto::ErrorResponse;
using dto_session::LeaveRequest;
using dto_session::LeaveResponse;
using dto_session::StartRequest;
using dto_session::StartResponse;

SessionController::SessionController(Session& session, const int client_id,
                                     Api& api, Receiver& receiver,
                                     ISessionEvents& handler,
                                     spdlog::logger* log)
    : Session::Listener(session),
      Receiver::Listener(receiver),
      log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler) {
    log->debug("controlling lobby");
}

void SessionController::on_start_request(const bool ready) {
    try {
        session.set_ready(client_id, ready);
        log->trace(ready ? "ready" : "not ready");
    } catch (std::runtime_error& e) {
        log->trace("could not set {}: {}", ready ? "ready" : "not ready",
                   e.what());
        api.reply_error(e.what());
    }
}

void SessionController::on_start_game(Game& game) {
    dispatcher.on_start_game(game);
}
