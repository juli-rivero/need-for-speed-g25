#include "server/client_handler/sender.h"

using dto::ResponseType;

Sender::Sender(ProtocolSender& sender) : sender(sender) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            sender << responses.pop() << ProtocolSender::send;
        } catch (ClosedQueue&) {
            return;
        } catch (ClosedProtocol&) {
            return;
        }
    }
}

void Sender::kill() {
    stop();
    if (not sender.is_stream_send_closed()) {
        sender.close_stream_send();
    }
    responses.close();
}

void Sender::send(const dto_session::SearchResponse& body) {
    responses.try_push({ResponseType::SearchResponse, body});
}

void Sender::send(const dto_session::JoinResponse& body) {
    responses.try_push({ResponseType::JoinResponse, body});
}

void Sender::send(const dto_session::LeaveResponse& body) {
    responses.try_push({ResponseType::LeaveResponse, body});
}

void Sender::send(const dto_lobby::StartResponse& body) {
    responses.try_push({ResponseType::StartResponse, body});
}

void Sender::send(const dto::ErrorResponse& body) {
    responses.try_push({ResponseType::ErrorResponse, body});
}
