
#include "MatchSession.h"

#include <limits>
#include <ranges>
#include <utility>

MatchSession::MatchSession(const YamlGameConfig& cfg, Box2DPhysicsWorld& world,
                           std::vector<std::string>&& raceFiles,
                           u_map<PlayerId, u_ptr<Player>>&& players,
                           std::vector<u_ptr<Wall>>&& buildings,
                           std::vector<u_ptr<BridgeSensor>>&& sensors,
                           RoadGraph&& roadGraph)
    : _cfg(cfg),
      _world(world),
      _roadGraph(std::move(roadGraph)),
      _raceFiles(std::move(raceFiles)),
      _players(std::move(players)),
      _buildings(std::move(buildings)),
      _sensors(std::move(sensors)) {
    spdlog::debug("MatchSession created");

    if (_raceFiles.empty()) {
        _state = MatchState::Finished;
        return;
    }

    _state = MatchState::Racing;
    startRace(0);
}

void MatchSession::update(float dt) {
    switch (_state) {
        case MatchState::Starting:
        case MatchState::Racing:
            for (auto& [id, player] : _players) {
                player->update(dt);
            }
            if (_traffic) _traffic->update(dt);

            _race->update(dt);

            if (_race->isFinished()) {
                startIntermission();
            }
            break;

        case MatchState::Intermission:
            _intermissionClock += dt;
            if (_intermissionClock >= _cfg.getIntermissionSec()) {
                endIntermissionAndPrepareNextRace();
            }
            break;

        case MatchState::Finished:
            break;
    }
}

void MatchSession::applyInput(PlayerId id, const CarInput& input) const {
    if (_race && _race->getState() != RaceState::Countdown)
        _players.at(id)->applyInput(input);
}

void MatchSession::applyCheat(const PlayerId id, const Cheat cheat) const {
    switch (cheat) {
        case Cheat::FinishRace:
            _players.at(id)->markFinished();
            break;
        case Cheat::DestroyAllCars:
            for (const auto& [_id, player] : _players) {
                auto* car = player->getCar();
                car->damage(std::numeric_limits<float>::max());
            }
            break;
        case Cheat::InfiniteHealth:
            _players.at(id)->getCar()->upgrade(
                UpgradeStat::Health, std::numeric_limits<float>::infinity());
            break;
    }
}

void MatchSession::applyUpgrade(const PlayerId id, const UpgradeStat upgrade) {
    auto& player = _players.at(id);

    const auto& data = _cfg.getUpgradesMap().at(upgrade);

    player->applyUpgrade(upgrade, data.delta, data.penalty);
}
