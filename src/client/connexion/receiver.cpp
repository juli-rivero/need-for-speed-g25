#include "client/connexion/receiver.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <stdexcept>

#include "common/dto/dto_session.h"

using dto::ResponseType;

Receiver::Receiver(ProtocolReceiver& receiver) : receiver(receiver) {}

Receiver::Listener::Listener(Receiver& receiver)
    : common::Listener<Receiver::Listener>(receiver.emitter) {}

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
    spdlog::debug("Receiver ended");
}
void Receiver::stop() {
    Thread::stop();
    if (not receiver.is_stream_recv_closed()) receiver.close_stream_recv();
}

void Receiver::delegate_response(const ResponseType& response) {
    spdlog::trace("received type response: {}", static_cast<int>(response));
    switch (response) {
        case ResponseType::JoinResponse: {
            auto join_response = receiver.get<dto_search::JoinResponse>();
            emitter.dispatch(&Listener::on_join_response, join_response.session,
                             join_response.carTypes);
            break;
        }
        case ResponseType::SearchResponse: {
            auto search_response = receiver.get<dto_search::SearchResponse>();
            emitter.dispatch(&Listener::on_search_response,
                             search_response.sessions);
            break;
        }
        case ResponseType::LeaveResponse: {
            receiver.get<dto_session::LeaveResponse>();
            emitter.dispatch(&Listener::on_leave_response);
            break;
        }
        case ResponseType::StartResponse: {
            receiver.get<dto_session::StartResponse>();
            emitter.dispatch(&Listener::on_start_response);
            break;
        }
        case ResponseType::SessionSnapshot: {
            const auto snapshot = receiver.get<dto_session::SessionSnapshot>();
            emitter.dispatch(&Listener::on_session_snapshot, snapshot.session,
                             snapshot.players);
            break;
        }
        default:
            throw std::runtime_error("Unknown response type");
    }
}
