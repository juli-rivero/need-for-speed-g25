#include "server/client_handler/controllers/search_controller.h"

#include <ranges>
#include <vector>

using dto::ErrorResponse;
using dto_search::JoinRequest;
using dto_search::JoinResponse;
using dto_search::SearchRequest;
using dto_search::SearchResponse;

using std::string;
using std::vector;

SearchController::SearchController(SessionsMonitor& sessions_monitor,
                                   const int client_id, Api& api,
                                   Receiver& receiver, ISearchEvents& handler,
                                   spdlog::logger* log)
    : Listener(receiver),
      log(log),
      sessions_monitor(sessions_monitor),
      client_id(client_id),
      api(api),
      dispatcher(handler) {
    log->debug("controlling browser");
}

void SearchController::on_join_request(const std::string& session_id) {
    try {
        Session& session = sessions_monitor.get_session(session_id);
        session.add_client(client_id);
        dispatcher.on_join_session(session);
        log->trace("added client to session");
        api.reply_joined(session.get_info(),
                         session.get_types_of_static_cars());
    } catch (const std::runtime_error& e) {
        log->trace("could not join session: {}", e.what());
        api.reply_error(e.what());
    }
}
void SearchController::on_search_request() {
    const auto sessions_ids = sessions_monitor.get_sessions_ids();
    vector<SessionInfo> sessions(sessions_ids.size());
    for (size_t i = 0; i < sessions_ids.size(); ++i) {
        try {
            Session& session = sessions_monitor.get_session(sessions_ids[i]);
            sessions[i] = session.get_info();
        } catch (const std::runtime_error&) {}
    }
    log->trace("found {} sessions", sessions.size());
    api.reply_search(sessions);
}
void SearchController::on_create_request(const SessionConfig& session_config) {
    sessions_monitor.create_session(session_config, client_id);
    log->trace("created session");
    on_join_request(session_config.name);
}
void SearchController::on_leave_request() {
    sessions_monitor.leave_session(client_id);
}
