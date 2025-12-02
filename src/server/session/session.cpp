#include "server/session/session.h"

#include <ctime>
#include <ranges>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

using spdlog::stdout_color_mt;

UserSetup::UserSetup(const std::string& name, const PlayerId id)
    : game_config(id, name, YamlGameConfig::DefaultCar) {}

void Session::Listener::subscribe(Session& s) {
    common::Listener<Session::Listener>::subscribe(s.emitter);
}

Session::Session(const SessionConfig& config, const PlayerId creator,
                 YamlGameConfig& yaml_config)
    : config(config),
      log(stdout_color_mt("Session " + config.name)),
      users_setup(1),
      game(nullptr),
      yaml_config(yaml_config),
      creator(creator),
      rand_generator(std::random_device{}()) {
    log->debug("Created");
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

void Session::add_client(const PlayerId client_id) {
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

void Session::remove_client(const PlayerId client_id) {
    std::lock_guard lock(mtx);
    if (not in_game()) users_setup.erase(client_id);
    log->debug("client {} left", client_id);
    notify_change();
}

bool Session::in_game() const { return game != nullptr; }
bool Session::full() const { return users_setup.size() >= config.maxPlayers; }
bool Session::empty() const { return users_setup.empty(); }

void Session::set_car(const PlayerId client_id, const CarType& car_name) {
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    if (not users_setup.contains(client_id))
        throw std::runtime_error("Client have not configuration");
    users_setup.at(client_id).game_config.carType = car_name;
    notify_change();
}

void Session::set_ready(const PlayerId client_id, const bool ready) {
    log->debug("client {} set ready to {}", client_id, ready);
    std::lock_guard lock(mtx);
    if (in_game()) throw std::runtime_error("Game already started");
    if (not users_setup.contains(client_id))
        throw std::runtime_error("Client have not configuration");
    users_setup.at(client_id).ready = ready;
    notify_change();
    if (all_ready()) start_game();
}

Session::~Session() {
    if (game != nullptr) {
        if (game->is_alive()) game->stop();
        game->join();
    }
    log->debug("Destroyed");
    spdlog::drop(log->name());
}

void Session::start_game() {
    log->debug("starting game");

    std::vector<PlayerConfig> players;
    players.reserve(users_setup.size());
    for (const auto& player : users_setup | std::views::values)
        players.push_back(player.game_config);

    std::vector<std::string> races;
    races.reserve(config.raceCount);
    const auto all_races = yaml_config.getRaces(config.city);
    for (int i = 0; i < config.raceCount; ++i) {
        const auto index = std::uniform_int_distribution<int>(
            0, all_races.size() - 1)(rand_generator);

        spdlog::info("race {}", all_races[index]);
        races.push_back(all_races[0]);  // TODO(juli): hacer carreras dinamicas
    }

    game =
        std::make_unique<GameSessionFacade>(yaml_config, races, players, log);
    CityInfo city_info = game->getCityInfo();
    city_info.name =
        config.city;  // TODO(juli): esto no se deberia hacer, cambiar

    std::vector<UpgradeChoice> upgrade_choices;
    const auto& choices_map = yaml_config.getUpgradesMap();
    upgrade_choices.reserve(choices_map.size());
    for (const auto& choice : choices_map | std::views::values)
        upgrade_choices.push_back(choice);
    emitter.dispatch(&Listener::on_start_game, *game, city_info,
                     game->getRaceInfo(), upgrade_choices);
    spdlog::trace("iniciando gameloop");
    game->start();
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
            .id = id,
            .name = player.game_config.name,
            .carType = player.game_config.carType,
            .isReady = player.ready,
            .isHost = id == creator,
        });
    }
    log->trace("sending update");
    for (const auto& player : players)
        log->trace("\t player is ready: {}", player.isReady);

    emitter.dispatch(&Listener::on_session_updated, config, players);
}
