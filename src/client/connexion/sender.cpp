#include "client/connexion/sender.h"

using dto::RequestType;

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

void Sender::stop() {
    Thread::stop();
    if (not sender.is_stream_send_closed()) sender.close_stream_send();
    responses.close();
}

void Sender::send(const dto_session::SearchRequest& body) {
    responses.try_push({RequestType::SearchRequest, body});
}

void Sender::send(const dto_session::JoinRequest& body) {
    responses.try_push({RequestType::JoinRequest, body});
}

void Sender::send(const dto_session::LeaveRequest& body) {
    responses.try_push({RequestType::LeaveRequest, body});
}

void Sender::send(const dto_lobby::StartRequest& body) {
    responses.try_push({RequestType::StartRequest, body});
}
