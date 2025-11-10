#include "server/client_handler/sender.h"

#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"

using dto::ErrorResponse;
using dto::ResponseType;
using dto_search::JoinResponse;
using dto_search::SearchResponse;
using dto_session::LeaveResponse;
using dto_session::StartResponse;

using std::string;
using std::vector;

Sender::Sender(ProtocolSender& sender, spdlog::logger* log)
    : sender(sender), log(log) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            auto response = responses.pop();
            log->trace("sending type response: {}",
                       static_cast<int>(response.type));
            sender << response << ProtocolSender::send;
        } catch (ClosedQueue&) {
            log->debug("Sender stopped by closed queue");
            return;
        } catch (ClosedProtocol&) {
            log->debug("Sender stopped by closed protocol");
            return;
        }
    }
}

void Sender::stop() {
    Thread::stop();
    if (not sender.is_stream_send_closed()) {
        sender.close_stream_send();
    }
    responses.close();
    log->debug("Sender stopped");
}

void Sender::reply_search(const vector<SessionInfo>& info) {
    log->trace("sending search response");
    responses.try_push({ResponseType::SearchResponse, SearchResponse{info}});
}

void Sender::reply_joined(const SessionInfo& session,
                          const vector<CarStaticInfo>& carTypes) {
    log->trace("sending join response");
    responses.try_push(
        {ResponseType::JoinResponse, JoinResponse{session, carTypes}});
}

void Sender::reply_left() {
    log->trace("sending leave response");
    responses.try_push({ResponseType::LeaveResponse, LeaveResponse{}});
}

void Sender::reply_started() {
    log->trace("sending start response");
    responses.try_push({ResponseType::StartResponse, StartResponse{}});
}

void Sender::reply_error(const string& message) {
    log->trace("sending error response");
    responses.try_push({ResponseType::ErrorResponse, ErrorResponse{message}});
}
