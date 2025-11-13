#include "client/connexion/sender.h"

#include <spdlog/spdlog.h>

using dto::RequestType;
using dto_search::CreateRequest;
using dto_search::JoinRequest;
using dto_search::SearchRequest;
using dto_session::LeaveRequest;
using dto_session::StartRequest;

Sender::Sender(ProtocolSender& sender) : sender(sender) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            auto response = responses.pop();
            spdlog::trace("sending type request: {}",
                          static_cast<int>(response.type));
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
    responses.try_push({RequestType::SearchRequest, SearchRequest{}});
}
void Sender::request_join_session(const std::string& session) {
    responses.try_push({RequestType::JoinRequest, JoinRequest{session}});
}
void Sender::request_create_session(const SessionConfig& config) {
    responses.try_push({RequestType::CreateRequest, CreateRequest{config}});
}
void Sender::request_leave_current_session() {
    responses.try_push({RequestType::LeaveRequest, LeaveRequest{}});
}
void Sender::request_start_session() {
    responses.try_push({RequestType::StartRequest, StartRequest{}});
}
