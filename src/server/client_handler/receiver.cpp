#include "server/client_handler/receiver.h"

#include <iostream>
#include <stdexcept>

#include "spdlog/spdlog.h"

using dto::RequestType;

Receiver::Receiver(ProtocolReceiver& receiver, RequestListener& listener,
                   spdlog::logger* log)
    : receiver(receiver), listener(listener), log(log) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            switch_and_dispatch_request(receiver.get<RequestType>());
        } catch (ClosedProtocol&) {
            log->trace("Protocol closed by client");
            return;
        } catch (std::runtime_error& e) {
            log->critical("Receiver error: {}", e.what());
        }
    }
}

void Receiver::stop() {
    Thread::stop();
    if (not receiver.is_stream_recv_closed()) receiver.close_stream_recv();
    log->trace("Receiver stopped");
}

void Receiver::switch_and_dispatch_request(const RequestType& request) const {
    switch (request) {
        case RequestType::JoinRequest:
            log->trace("Received JoinRequest");
            listener.on(receiver.get<dto_search::JoinRequest>());
            break;
        case RequestType::SearchRequest:
            log->trace("Received SearchRequest");
            listener.on(receiver.get<dto_search::SearchRequest>());
            break;
        case RequestType::LeaveRequest:
            log->trace("Received LeaveRequest");
            listener.on(receiver.get<dto_session::LeaveRequest>());
            break;
        case RequestType::StartRequest:
            log->trace("Received StartRequest");
            listener.on(receiver.get<dto_session::StartRequest>());
            break;
        default:
            throw std::runtime_error("Unknown request type");
    }
}
