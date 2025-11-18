#include "server/client_handler/receiver.h"

#include <stdexcept>

#include "spdlog/spdlog.h"

Receiver::Receiver(ProtocolReceiver& receiver, spdlog::logger* log)
    : receiver(receiver), log(log) {}

void Receiver::Listener::subscribe(Receiver& r) {
    common::Listener<Receiver::Listener>::subscribe(r.emitter);
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

// -------------------- Game requests --------------------
void Receiver::recv(const dto_game::TurnRequest& request) {
    log->trace("Received TurnRequest: dir={}, turn={}",
               static_cast<int>(request.direction), request.turn);
    switch (request.direction) {
        case TurnDirection::Left:
            if (request.turn)
                emitter.dispatch(&Listener::on_turn_left);
            else
                emitter.dispatch(&Listener::on_stop_turning_left);
            break;
        case TurnDirection::Right:
            if (request.turn)
                emitter.dispatch(&Listener::on_turn_right);
            else
                emitter.dispatch(&Listener::on_stop_turning_right);
            break;
        default:
            throw std::runtime_error("Invalid turn direction");
    }
}

void Receiver::recv(const dto_game::AccelerateRequest& request) {
    log->trace("Received AccelerateRequest: {}", request.accelerate);
    if (request.accelerate)
        emitter.dispatch(&Listener::on_accelerate);
    else
        emitter.dispatch(&Listener::on_stop_accelerating);
}

void Receiver::recv(const dto_game::ReverseRequest& request) {
    log->trace("Received ReverseRequest: {}", request.reverse);
    if (request.reverse)
        emitter.dispatch(&Listener::on_reverse);
    else
        emitter.dispatch(&Listener::on_stop_reversing);
}

void Receiver::recv(const dto_game::UseBoostRequest& request) {
    log->trace("Received UseBoostRequest: {}", request.useBoost);
    if (request.useBoost) {
        emitter.dispatch(&Listener::on_nitro);
    }
}
