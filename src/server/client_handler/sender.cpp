#include "server/client_handler/sender.h"

#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"

using dto::ErrorResponse;
using dto_search::JoinResponse;
using dto_search::SearchResponse;
using dto_session::LeaveResponse;
using dto_session::SessionSnapshot;
using dto_session::StartResponse;

using std::string;
using std::vector;

Sender::Sender(ProtocolSender& sender, spdlog::logger* log)
    : sender(sender), log(log) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            // Try to put in the same packet as match responses as posible,
            // blocking first response, as a packet needs at least 1 response,
            // and adding consequent packets if there are.
            dto::Response response = responses.pop();
            do {
                sender << response;
            } while (responses.try_pop(response));
            sender.send();
        } catch (ClosedQueue&) {
            log->debug("Sender stopped by closed queue");
            return;
        } catch (ClosedProtocol&) {
            log->debug("Sender stopped by closed protocol");
            return;
        }
    }
}

void Sender::stop() {
    Thread::stop();
    if (not sender.is_stream_send_closed()) {
        sender.close_stream_send();
    }
    responses.close();
    log->debug("Sender stopped");
}

void Sender::reply_search(const vector<SessionInfo>& info) {
    log->trace("sending search response");
    responses.try_push(SearchResponse{info});
}

void Sender::reply_joined(const SessionInfo& session,
                          const vector<CarInfo>& carTypes) {
    log->trace("sending join response");
    responses.try_push(JoinResponse{session, carTypes});
}

void Sender::reply_left() {
    log->trace("sending leave response");
    responses.try_push(LeaveResponse{});
}

void Sender::reply_error(const string& message) {
    log->trace("sending error response");
    responses.try_push(ErrorResponse{message});
}
void Sender::send_session_snapshot(const SessionConfig& config,
                                   const std::vector<PlayerInfo>& players) {
    log->trace("sending session snapshot");
    responses.try_push(SessionSnapshot{config, players});
}
void Sender::notify_game_started(const CityInfo& city_info,
                                 const RaceInfo& first_race) {
    log->trace("sending start response");
    responses.try_push(StartResponse{city_info, first_race});
}

void Sender::send_game_static_snapshot(const RaceInfo& race) {
    log->trace("sending game static snapshot");
    responses.try_push(dto_game::GameStaticSnapshot{race});
}

void Sender::send_game_snapshot(const GameSnapshot& snapshot) {
    log->trace("sending game snapshot");
    responses.try_push(dto_game::GameSnapshotPacket{snapshot});
}
void Sender::send_collision_event(const CollisionEvent& event) {
    log->trace("sending collision event");
    responses.try_push(dto_game::EventPacket{event});
}
