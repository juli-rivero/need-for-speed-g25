#include "server/client_handler/sender.h"

using dto::ResponseType;

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
    responses.try_push(
        {ResponseType::SearchResponse, dto_search::SearchResponse{info}});
}

void Sender::reply_joined() {
    log->trace("sending join response");
    responses.try_push(
        {ResponseType::JoinResponse, dto_search::JoinResponse{}});
}

void Sender::reply_created() {
    log->trace("sending create response");
    responses.try_push(
        {ResponseType::CreateResponse, dto_search::CreateResponse{}});
}

void Sender::reply_left() {
    log->trace("sending leave response");
    responses.try_push(
        {ResponseType::LeaveResponse, dto_session::LeaveResponse{}});
}

void Sender::reply_started() {
    log->trace("sending start response");
    responses.try_push(
        {ResponseType::StartResponse, dto_session::StartResponse{}});
}

void Sender::reply_error(const string& message) {
    log->trace("sending error response");
    responses.try_push(
        {ResponseType::ErrorResponse, dto::ErrorResponse{message}});
}
