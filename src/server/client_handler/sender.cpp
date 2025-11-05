#include "server/client_handler/sender.h"

#include "spdlog/spdlog.h"

using dto::ResponseType;

Sender::Sender(ProtocolSender& sender, spdlog::logger* log)
    : sender(sender), log(log) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            sender << responses.pop() << ProtocolSender::send;
        } catch (ClosedQueue&) {
            log->trace("Sender stopped by closed queue");
            return;
        } catch (ClosedProtocol&) {
            log->trace("Sender stopped by closed protocol");
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
    log->trace("Sender stopped");
}

void Sender::send(const dto_session::SearchResponse& body) {
    log->trace("sending search response");
    responses.try_push({ResponseType::SearchResponse, body});
}

void Sender::send(const dto_session::JoinResponse& body) {
    log->trace("sending join response");
    responses.try_push({ResponseType::JoinResponse, body});
}

void Sender::send(const dto_session::LeaveResponse& body) {
    log->trace("sending leave response");
    responses.try_push({ResponseType::LeaveResponse, body});
}

void Sender::send(const dto_lobby::StartResponse& body) {
    log->trace("sending start response");
    responses.try_push({ResponseType::StartResponse, body});
}

void Sender::send(const dto::ErrorResponse& body) {
    log->trace("sending error response");
    responses.try_push({ResponseType::ErrorResponse, body});
}
