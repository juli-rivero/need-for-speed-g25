#include "client/connexion/receiver.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <stdexcept>

#include "common/dto/dto_session.h"

Receiver::Receiver(ProtocolReceiver& receiver) : receiver(receiver) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            const auto response = receiver.get<dto::Response>();
            std::visit([this](auto& body) { recv(body); }, response);
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

void Receiver::Listener::subscribe(Receiver& r) {
    common::Listener<Receiver::Listener>::subscribe(r.emitter);
}
void Receiver::recv(const dto::ErrorResponse& response) {
    spdlog::trace("received error response");
    emitter.dispatch(&Listener::on_error_response, response.message);
}
void Receiver::recv(const dto_search::SearchResponse& response) {
    spdlog::trace("received search response");
    emitter.dispatch(&Listener::on_search_response, response.sessions);
}
void Receiver::recv(const dto_search::JoinResponse& response) {
    spdlog::trace("received join response");
    emitter.dispatch(&Listener::on_join_response, response.session,
                     response.carTypes);
}
void Receiver::recv(const dto_session::LeaveResponse&) {
    spdlog::trace("received leave response");
    emitter.dispatch(&Listener::on_leave_response);
}
void Receiver::recv(const dto_session::StartResponse&) {
    spdlog::trace("received start response");
    emitter.dispatch(&Listener::on_start_game);
}
void Receiver::recv(const dto_session::SessionSnapshot& snapshot) {
    spdlog::trace("received session snapshot");
    emitter.dispatch(&Listener::on_session_snapshot, snapshot.session,
                     snapshot.players);
}
void Receiver::recv(const dto_game::GameSnapshot& snapshot) {
    spdlog::trace("received game snapshot");
    emitter.dispatch(&Listener::on_game_snapshot, snapshot.raceTimeLeft,
                     snapshot.players);
}
