#include "server/client_handler/controllers/session_controller.h"

#include <ranges>
#include <vector>

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
                                     const YamlGameConfig& game_config)
    : log(log),
      client_id(client_id),
      api(api),
      dispatcher(handler),
      session(session) {
    Receiver::Listener::subscribe(receiver);
    Session::Listener::subscribe(session);
    session.add_client(client_id);
    const auto& toDisplayInfo = game_config.getCarDisplayInfoMap();
    const auto& toStats = game_config.getCarStaticStatsMap();

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

void SessionController::on_start_game(GameSessionFacade& game,
                                      const CityInfo& city_info,
                                      const RaceInfo& race_info) {
    try {
        dispatcher.on_start_game(game);
        api.notify_game_started(city_info, race_info);
    } catch (std::exception& e) {
        log->warn("could not start game: {}", e.what());
        api.reply_error(e.what());
    }
}
