#include "server/client_handler/receiver.h"

#include <iostream>
#include <stdexcept>

using dto::RequestType;

Receiver::Receiver(ProtocolReceiver& receiver, RequestListener& listener)
    : receiver(receiver), listener(listener) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            switch_and_dispatch_request(receiver.get<RequestType>());
        } catch (ClosedProtocol&) {
            return;
        } catch (std::runtime_error& e) {
            std::cerr << "Error while processing client command: " << e.what()
                      << std::endl;
        }
    }
}

void Receiver::kill() {
    stop();
    if (not receiver.is_stream_recv_closed()) receiver.close_stream_recv();
}

void Receiver::switch_and_dispatch_request(const RequestType& request) const {
    switch (request) {
        case RequestType::JoinRequest:
            listener.on(receiver.get<dto_session::JoinRequest>());
            break;
        case RequestType::LeaveRequest:
            listener.on(receiver.get<dto_session::LeaveRequest>());
            break;
        case RequestType::SearchRequest:
            listener.on(receiver.get<dto_session::SearchRequest>());
            break;
        case RequestType::StartRequest:
            listener.on(receiver.get<dto_lobby::StartRequest>());
            break;
        default:
            throw std::runtime_error("Unknown request type");
    }
}
