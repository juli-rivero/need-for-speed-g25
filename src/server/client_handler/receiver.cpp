#include "server/client_handler/receiver.h"

#include <stdexcept>

#include "spdlog/spdlog.h"

Receiver::Receiver(ProtocolReceiver& receiver, spdlog::logger* log)
    : receiver(receiver), log(log) {}

Receiver::Listener::Listener(Receiver& receiver)
    : common::Listener<Receiver::Listener>(receiver.emitter) {
    receiver.log->trace("new listener added");
}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            const auto request = receiver.get<dto::Request>();
            std::visit([this](auto& body) { recv(body); }, request);
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

void Receiver::recv(const dto_search::JoinRequest& request) {
    log->trace("Received JoinRequest");
    emitter.dispatch(&Listener::on_join_request, request.session_id);
}
void Receiver::recv(const dto_search::SearchRequest&) {
    log->trace("Received SearchRequest");
    emitter.dispatch(&Listener::on_search_request);
}
void Receiver::recv(const dto_search::CreateRequest& request) {
    log->trace("Received CreateRequest");
    emitter.dispatch(&Listener::on_create_request, request.config);
}
void Receiver::recv(const dto_session::LeaveRequest&) {
    log->trace("Received LeaveRequest");
    emitter.dispatch(&Listener::on_leave_request);
}
void Receiver::recv(const dto_session::StartRequest& request) {
    log->trace("Received StartRequest");
    emitter.dispatch(&Listener::on_start_request, request.ready);
}
void Receiver::recv(const dto_session::ChooseCarRequest& request) {
    log->trace("Received ChooseCarRequest");
    emitter.dispatch(&Listener::on_choose_car, request.car_name);
}
