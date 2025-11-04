#include "client/connexion/receiver.h"

#include <iostream>
#include <stdexcept>

using dto::ResponseType;

Receiver::Receiver(ProtocolReceiver& receiver, ResponseListener& listener)
    : receiver(receiver), listener(listener) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            delegate_response(receiver.get<ResponseType>());
        } catch (ClosedProtocol&) {
            return;
        } catch (std::runtime_error& e) {
            std::cerr << "Error while processing client command: " << e.what()
                      << std::endl;
        }
    }
}
void Receiver::stop() {
    Thread::stop();
    if (not receiver.is_stream_recv_closed()) receiver.close_stream_recv();
}

void Receiver::delegate_response(const ResponseType& request) const {
    switch (request) {
        case ResponseType::JoinResponse:
            listener.recv(receiver.get<dto_session::JoinResponse>());
            break;
        case ResponseType::LeaveResponse:
            listener.recv(receiver.get<dto_session::LeaveResponse>());
            break;
        case ResponseType::SearchResponse:
            listener.recv(receiver.get<dto_session::SearchResponse>());
            break;
        case ResponseType::StartResponse:
            listener.recv(receiver.get<dto_lobby::StartResponse>());
            break;
        default:
            throw std::runtime_error("Unknown request type");
    }
}
