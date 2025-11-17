#include "client/connexion/sender.h"

#include <spdlog/spdlog.h>

using dto_search::CreateRequest;
using dto_search::JoinRequest;
using dto_search::SearchRequest;
using dto_session::ChooseCarRequest;
using dto_session::LeaveRequest;
using dto_session::StartRequest;

Sender::Sender(ProtocolSender& sender) : sender(sender) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            auto response = responses.pop();
            sender << response << ProtocolSender::send;
        } catch (ClosedQueue&) {
            return;
        } catch (ClosedProtocol&) {
            return;
        }
    }
    spdlog::debug("sender ended");
}

void Sender::stop() {
    Thread::stop();
    if (not sender.is_stream_send_closed()) sender.close_stream_send();
    responses.close();
}

void Sender::request_search_all_sessions() {
    spdlog::trace("requesting search all sessions");
    responses.try_push(SearchRequest{});
}
void Sender::request_join_session(const std::string& session) {
    spdlog::trace("requesting join session {}", session);
    responses.try_push(JoinRequest{session});
}
void Sender::request_create_and_join_session(const SessionConfig& config) {
    spdlog::trace("requesting create and join session {}", config.name);
    responses.try_push(CreateRequest{config});
}
void Sender::request_leave_current_session() {
    spdlog::trace("requesting leave current session");
    responses.try_push(LeaveRequest{});
}
void Sender::set_ready(const bool ready) {
    spdlog::trace("setting ready to {}", ready);
    responses.try_push(StartRequest{ready});
}
void Sender::choose_car(const std::string& car_name) {
    spdlog::trace("choosing car {}", car_name);
    responses.try_push(ChooseCarRequest{car_name});
}
