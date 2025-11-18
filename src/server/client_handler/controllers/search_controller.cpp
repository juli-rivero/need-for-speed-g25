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
                                   const PlayerId client_id, Api& api,
                                   Receiver& receiver, ISearchEvents& handler,
                                   spdlog::logger* log)
    : log(log),
      sessions_monitor(sessions_monitor),
      client_id(client_id),
      api(api),
      dispatcher(handler) {
    log->debug("controlling browser");
    Listener::subscribe(receiver);
}
SearchController::~SearchController() { Listener::unsubscribe(); }

void SearchController::on_join_request(const std::string& session_id) {
    try {
        Session& session = sessions_monitor.get_session(session_id);
        session.add_client(client_id);
        dispatcher.on_join_session(session);
        log->trace("added client to session");
    } catch (const std::exception& e) {
        log->trace("could not join session: {}", e.what());
        api.reply_error(e.what());
    }
}
void SearchController::on_search_request() {
    try {
        const auto sessions_ids = sessions_monitor.get_sessions_ids();
        vector<SessionInfo> sessions;
        sessions.reserve(sessions_ids.size());
        for (const auto& session_id : sessions_ids) {
            try {
                Session& session = sessions_monitor.get_session(session_id);
                sessions.push_back(session.get_info());
            } catch (const std::exception&) {}
        }
        log->trace("found {} sessions", sessions.size());
        api.reply_search(sessions);
    } catch (std::exception& e) {
        log->warn("could not search sessions: {}", e.what());
        api.reply_error(e.what());
    }
}
void SearchController::on_create_request(const SessionConfig& session_config) {
    try {
        log->trace("creating session");
        sessions_monitor.create_session(session_config, client_id);
        log->trace("created session");
        on_join_request(session_config.name);
    } catch (std::exception& e) {
        log->warn("could not create session: {}", e.what());
        api.reply_error(e.what());
    }
}
