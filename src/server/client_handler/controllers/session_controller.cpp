#include "server/client_handler/controllers/session_controller.h"

#include <ranges>
#include <vector>

#include "server/config/convertor.h"

using dto::ErrorResponse;
using dto_session::LeaveRequest;
using dto_session::LeaveResponse;
using dto_session::StartRequest;
using dto_session::StartResponse;

using std::vector;

SessionController::SessionController(Session& session, const PlayerId client_id,
                                     Api& api, Receiver& receiver,
                                     ISessionEvents& handler,
                                     spdlog::logger* log,
                                     const YamlGameConfig& cfg)
    : log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler),
      session(session),
      cfg(cfg) {
    Receiver::Listener::subscribe(receiver);
    Session::Listener::subscribe(session);
    session.add_client(client_id);
    const auto& toDisplayInfo = cfg.getCarDisplayInfoMap();
    const auto& toStats = cfg.getCarStaticStatsMap();

    std::vector<CarInfo> cars;
    cars.reserve(toDisplayInfo.size());
    for (const auto type : toDisplayInfo | std::views::keys) {
        cars.push_back({type, toDisplayInfo.at(type), toStats.at(type)});
    }
    api.reply_joined(session.get_info(), cars);
    log->debug("controlling session");
}

SessionController::~SessionController() {
    Receiver::Listener::unsubscribe();
    Session::Listener::unsubscribe();
    api.reply_left();
    session.remove_client(client_id);
    log->trace("left session");
}

void SessionController::on_start_request(const bool ready) {
    try {
        session.set_ready(client_id, ready);
        log->trace(ready ? "ready" : "not ready");
    } catch (std::exception& e) {
        log->warn("could not set {}: {}", ready ? "ready" : "not ready",
                  e.what());
        api.reply_error(e.what());
    }
}
void SessionController::on_choose_car(const CarType& car_type) {
    try {
        session.set_car(client_id, car_type);
    } catch (std::exception& e) {
        log->warn("could not set car: {}", e.what());
        api.reply_error(e.what());
    }
}
void SessionController::on_leave_request() {
    try {
        dispatcher.on_leave_session();
    } catch (std::exception& e) {
        log->warn("could not send session snapshot: {}", e.what());
        api.reply_error(e.what());
    }
}

void SessionController::on_session_updated(
    const SessionConfig& config, const std::vector<PlayerInfo>& players) {
    try {
        api.send_session_snapshot(config, players);
    } catch (std::exception& e) {
        log->warn("could not send session snapshot: {}", e.what());
        api.reply_error(e.what());
    }
}

void SessionController::on_start_game(
    GameSessionFacade& game, const CityInfo& city_info,
    const RaceInfo& race_info, const std::vector<UpgradeChoice>& choices) {
    try {
        dispatcher.on_start_game(game);
        Convertor conv(cfg.getPixelsPerMeter());

        CityInfo transformed_city_info = city_info;
        for (auto& wall : transformed_city_info.walls)
            wall = conv.toPixels(wall);
        for (auto& bridge : transformed_city_info.bridges)
            bridge = conv.toPixels(bridge);
        for (auto& overpass : transformed_city_info.overpasses)
            overpass = conv.toPixels(overpass);
        for (auto& railing : transformed_city_info.railings)
            railing = conv.toPixels(railing);
        for (auto& sensor : transformed_city_info.upper_sensors)
            sensor = conv.toPixels(sensor);
        for (auto& sensor : transformed_city_info.lower_sensors)
            sensor = conv.toPixels(sensor);
        RaceInfo transformed_race_info = race_info;
        for (auto& checkpoint : transformed_race_info.checkpoints) {
            checkpoint.bound = conv.toPixels(checkpoint.bound);
            checkpoint.angle = conv.toDegrees(checkpoint.angle);
        }
        for (auto& spawn_point : transformed_race_info.spawnPoints) {
            spawn_point.pos = conv.toPixels(spawn_point.pos);
            spawn_point.angle = conv.toDegrees(spawn_point.angle);
        }
        api.notify_game_started(transformed_city_info, transformed_race_info,
                                choices);
    } catch (std::exception& e) {
        log->warn("could not start game: {}", e.what());
        api.reply_error(e.what());
    }
}
