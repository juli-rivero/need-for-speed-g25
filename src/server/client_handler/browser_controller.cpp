#include "server/client_handler/browser_controller.h"

#include <string>

using dto::ErrorResponse;
using dto_session::JoinRequest;
using dto_session::JoinResponse;
using dto_session::LeaveRequest;
using dto_session::LeaveResponse;
using dto_session::SearchRequest;
using dto_session::SearchResponse;

BrowserController::BrowserController(SessionsMonitor& sessions_monitor,
                                     const int client_id, Sender& sender,
                                     IBrowserEvents& handler)
    : sessions_monitor(sessions_monitor),
      client_id(client_id),
      sender(sender),
      dispatcher(handler) {}

void BrowserController::on(const LeaveRequest&) const {
    try {
        sessions_monitor.leave_session(client_id);
        sender.send(LeaveResponse{});
    } catch (std::out_of_range&) {
        sender.send(ErrorResponse{});
    }
}

void BrowserController::on(const JoinRequest& lobby_join_request) const {
    const std::string& session_id = lobby_join_request.session_id;
    try {
        Session& session = sessions_monitor.get_session(session_id);
        session.add_client(client_id);
        dispatcher.on_join_session(session);
        sender.send(JoinResponse{});
    } catch (const std::runtime_error& e) {
        sender.send(ErrorResponse{e.what()});
    }
}

void BrowserController::on(const SearchRequest&) const {
    SearchResponse response;
    for (const std::string& session_id : sessions_monitor.get_sessions_ids()) {
        try {
            Session& session = sessions_monitor.get_session(session_id);
            uint16_t players = session.get_users_count();
            response.sessions.emplace_back(session_id, players);
        } catch (const std::runtime_error&) {
        }
    }
    sender.send(response);
}
