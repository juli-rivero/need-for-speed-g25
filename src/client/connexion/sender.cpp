#include "client/connexion/sender.h"

#include <spdlog/spdlog.h>

using dto_game::AccelerateRequest;
using dto_game::ReverseRequest;
using dto_game::TurnRequest;
using dto_game::UseBoostRequest;
using dto_search::CreateRequest;
using dto_search::JoinRequest;
using dto_search::SearchRequest;
using dto_session::ChooseCarRequest;
using dto_session::LeaveRequest;
using dto_session::StartRequest;

Sender::Sender(ProtocolSender& sender) : sender(sender) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            // Try to put in the same packet as match responses as posible,
            // blocking first response, as a packet needs at least 1 response,
            // and adding consequent packets if there are.
            dto::Request request = requests.pop();
            do {
                sender << request;
            } while (requests.try_pop(request));
            sender.send();
        } catch (ClosedQueue&) {
            return;
        } catch (ClosedProtocol&) {
            return;
        }
    }
    spdlog::debug("sender ended");
}

void Sender::stop() {
    Thread::stop();
    if (not sender.is_stream_send_closed()) sender.close_stream_send();
    requests.close();
}

void Sender::request_search_all_sessions() {
    spdlog::trace("requesting search all sessions");
    requests.try_push(SearchRequest{});
}
void Sender::request_join_session(const std::string& session) {
    spdlog::trace("requesting join session {}", session);
    requests.try_push(JoinRequest{session});
}
void Sender::request_static_session_data() {
    spdlog::trace("requesting static session data");
    requests.try_push(dto_search::StaticSessionDataRequest{});
}
void Sender::request_create_and_join_session(const SessionConfig& config) {
    spdlog::trace("requesting create and join session {}", config.name);
    requests.try_push(CreateRequest{config});
}
void Sender::request_leave_current_session() {
    spdlog::trace("requesting leave current session");
    requests.try_push(LeaveRequest{});
}
void Sender::set_ready(const bool ready) {
    spdlog::trace("setting ready to {}", ready);
    requests.try_push(StartRequest{ready});
}
void Sender::choose_car(const CarType& car_type) {
    spdlog::trace("choosing car {}", static_cast<int>(car_type));
    requests.try_push(ChooseCarRequest{car_type});
}

// Game
void Sender::start_turning(TurnDirection dir) {
    spdlog::trace("start turning {}", static_cast<int>(dir));
    requests.try_push(TurnRequest{dir, true});
}
void Sender::stop_turning(TurnDirection dir) {
    spdlog::trace("stop turning {}", static_cast<int>(dir));
    requests.try_push(TurnRequest{dir, false});
}
void Sender::start_accelerating() {
    spdlog::trace("start accelerating");
    requests.try_push(AccelerateRequest{true});
}
void Sender::stop_accelerating() {
    spdlog::trace("stop accelerating");
    requests.try_push(AccelerateRequest{false});
}
void Sender::start_breaking() {
    spdlog::trace("start reversing/braking");
    requests.try_push(ReverseRequest{true});
}
void Sender::stop_breaking() {
    spdlog::trace("stop reversing/braking");
    requests.try_push(ReverseRequest{false});
}
void Sender::start_using_nitro() {
    spdlog::trace("use nitro");
    requests.try_push(UseBoostRequest{true});
}

void Sender::cheat(Cheat cheat) {
    spdlog::trace("sending cheat {}", static_cast<int>(cheat));
    requests.try_push(dto_game::CheatMessage{cheat});
}
