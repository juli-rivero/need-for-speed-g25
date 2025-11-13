#include "server/session/session.h"

#include <ctime>
#include <ranges>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

using spdlog::stdout_color_mt;

UserSetup::UserSetup(const std::string& name, const int id)
    : game_config(static_cast<PlayerId>(id), name, YamlGameConfig::DefaultCar) {
}

Session::Listener::Listener(Session& session)
    : common::Listener<Session::Listener>(session.emitter), session(session) {
    session.log->trace("new listener added");
}

Session::Session(const SessionConfig& config, const int creator,
                 YamlGameConfig& yaml_config)
    : config(config),
      log(stdout_color_mt("Session " + config.name)),
      users_setup(1),
      game(yaml_config),
      yaml_config(yaml_config),
      creator(creator) {
    log->debug("Created");
    std::srand(std::time(nullptr));
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
    users_setup.emplace(
        client_id,
        UserSetup("Player " + std::to_string(client_id),
                  client_id));  // # TODO(juli): poner nombre del usuario
    log->debug("client {} joined", client_id);
    notify_change();
}

void Session::remove_client(const int client_id) {
    std::lock_guard lock(mtx);
    if (not in_game()) users_setup.erase(client_id);
    log->debug("client {} left", client_id);
    notify_change();
}

bool Session::in_game() const { return game.isRunning(); }
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

void Session::set_car(const int client_id, const std::string& car_name) {
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    if (not users_setup.contains(client_id))
        throw std::runtime_error("Client have not configuration");
    users_setup.at(client_id).game_config.carTypeName = car_name;
    notify_change();
}

void Session::set_ready(const int client_id, const bool ready) {
    log->debug("client {} set ready to {}", client_id, ready);
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    if (not users_setup.contains(client_id))
        throw std::runtime_error("Client have not configuration");
    users_setup.at(client_id).ready = ready;
    notify_change();
    if (all_ready()) start_game();
}

Session::~Session() { log->debug("Destroyed"); }

void Session::start_game() {
    std::lock_guard lock(mtx);

    std::vector<PlayerConfig> players;
    players.reserve(users_setup.size());
    for (const auto& player : users_setup | std::views::values)
        players.push_back(player.game_config);

    std::vector<RaceDefinition> races;
    races.reserve(config.raceCount);
    const auto all_races = yaml_config.getRaces(config.city);
    for (int i = 0; i < config.raceCount; ++i) {
        races.push_back(all_races[std::rand() % all_races.size()]);
    }

    game.start({}, players);
    emitter.dispatch(&Listener::on_start_game, game);
}

bool Session::all_ready() const {
    for (const auto& user : users_setup | std::views::values) {
        if (not user.ready) return false;
    }
    return true;
}

void Session::notify_change() {
    std::vector<PlayerInfo> players;
    players.reserve(users_setup.size());
    for (const auto& [id, player] : users_setup) {
        players.push_back({
            .name = player.game_config.name,
            .carType = YamlGameConfig::getCarSpriteType(
                player.game_config.carTypeName),
            .isReady = player.ready,
            .isHost = id == creator,
        });
    }
    emitter.dispatch(&Listener::on_session_updated, config, players);
}
