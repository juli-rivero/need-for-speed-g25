#include "server/session/session.h"

#include <ranges>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

using spdlog::stdout_color_mt;

Session::Listener::Listener(Session& session)
    : common::Listener<Session::Listener>(session.emitter), session(session) {
    session.log->trace("new listener added");
}

Session::Session(const SessionConfig& config, const int creator,
                 YamlGameConfig& yaml_config)
    : config(config),
      log(stdout_color_mt("Session " + config.name)),
      users_setup(1),
      game(nullptr),
      yaml_config(yaml_config) {
    log->debug("Created");
    add_client(creator);
}

SessionInfo Session::get_info() {
    std::lock_guard lock(mtx);
    auto status = SessionStatus::Waiting;
    if (full()) status = SessionStatus::Full;
    if (in_game()) status = SessionStatus::Playing;
    return {
        .name = config.name,
        .maxPlayers = config.maxPlayers,
        .raceCount = config.raceCount,
        .city = config.city,
        .currentPlayers = static_cast<uint8_t>(users_setup.size()),
        .status = status,
    };
}

void Session::add_client(const int client_id) {
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    if (full()) {
        throw std::runtime_error("Session is full");
    }
    users_setup[client_id] = {};
    log->debug("client {} joined", client_id);
}

void Session::remove_client(const int client_id) {
    std::lock_guard lock(mtx);
    if (not in_game()) users_setup.erase(client_id);
    log->debug("client {} left", client_id);
}

bool Session::in_game() const { return game != nullptr; }
bool Session::full() const { return users_setup.size() >= config.maxPlayers; }

std::vector<CarStaticInfo> Session::get_types_of_static_cars() const {
    const auto cars = yaml_config.getCarTypes();
    std::vector<CarStaticInfo> types_of_static_cars;
    for (const auto& car : cars) {
        types_of_static_cars.push_back({
            .type = YamlGameConfig::getCarSpriteType(car.name),
            .name = car.name,
            .description = car.description,
            .height = car.height,
            .width = car.width,
            .maxSpeed = car.maxSpeed,
            .acceleration = car.acceleration,
            .mass = car.mass,
            .control = car.control,
            .health = car.health,
        });
    }
    return types_of_static_cars;
}

void Session::set_ready(const int client_id, const bool ready) {
    log->debug("client {} set ready to {}", client_id, ready);
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    users_setup.at(client_id).ready = ready;
    if (all_ready()) start_game();
}

Session::~Session() {
    std::unique_lock lock(mtx);
    delete game;
    log->debug("Destroyed");
}

void Session::start_game() {
    std::lock_guard lock(mtx);
    game = new Game(users_setup, log.get());
    emitter.dispatch(&Listener::on_start_game, *game);
}

bool Session::all_ready() const {
    for (const auto& user : users_setup | std::views::values) {
        if (not user.ready) return false;
    }
    return true;
}
