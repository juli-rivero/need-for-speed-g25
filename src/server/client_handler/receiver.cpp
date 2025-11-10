#include "server/client_handler/receiver.h"

#include <stdexcept>

#include "spdlog/spdlog.h"

using dto::RequestType;

Receiver::Receiver(ProtocolReceiver& receiver, spdlog::logger* log)
    : receiver(receiver), log(log) {}

Receiver::Listener::Listener(Receiver& receiver)
    : common::Listener<Receiver::Listener>(receiver.emitter) {
    receiver.log->trace("new listener added");
}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            switch_and_dispatch_request(receiver.get<RequestType>());
        } catch (ClosedProtocol&) {
            log->debug("Protocol closed by client");
            return;
        } catch (std::runtime_error& e) {
            log->critical("Receiver error: {}", e.what());
        }
    }
}

void Receiver::stop() {
    Thread::stop();
    if (not receiver.is_stream_recv_closed()) receiver.close_stream_recv();
    log->debug("Receiver stopped");
}

void Receiver::switch_and_dispatch_request(const RequestType& request) {
    log->trace("received type request: {}", static_cast<int>(request));
    switch (request) {
        case RequestType::JoinRequest: {
            log->trace("Received JoinRequest");
            auto join_request = receiver.get<dto_search::JoinRequest>();
            emitter.dispatch(&Listener::on_join_request,
                             join_request.session_id);
            break;
        }
        case RequestType::SearchRequest: {
            log->trace("Received SearchRequest");
            receiver.get<dto_search::SearchRequest>();
            emitter.dispatch(&Listener::on_search_request);
            break;
        }
        case RequestType::CreateRequest: {
            log->trace("Received CreateRequest");
            auto create_request = receiver.get<dto_search::CreateRequest>();
            emitter.dispatch(&Listener::on_create_request,
                             create_request.config);
            break;
        }
        case RequestType::LeaveRequest: {
            log->trace("Received LeaveRequest");
            receiver.get<dto_session::LeaveRequest>();
            emitter.dispatch(&Listener::on_leave_request);
            break;
        }
        case RequestType::StartRequest: {
            log->trace("Received StartRequest");
            auto start_request = receiver.get<dto_session::StartRequest>();
            emitter.dispatch(&Listener::on_start_request, start_request.ready);
            break;
        }
        default:
            throw std::runtime_error("Unknown request type");
    }
}
