
#include "MatchSession.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>

#include "../../config/MapLoader.h"
#include "../physics/EntityFactory.h"
#include "server/session/NPC/TrafficSystem.h"

MatchSession::MatchSession(const YamlGameConfig& cfg,
                           std::vector<std::string> raceFiles,
                           Box2DPhysicsWorld& world,
                           std::vector<PlayerConfig> playersCfg)
    : _cfg(cfg),
      _world(world),
      _raceFiles(std::move(raceFiles)),
      _playerConfigs(std::move(playersCfg)),
      _upgradeSystem(cfg) {
    if (_raceFiles.empty()) {
        _state = MatchState::Finished;
        return;
    }
    _state = MatchState::Racing;
    startRace(0);
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    std::vector<std::unique_ptr<Wall>> buildings;
    std::vector<Bound> bridges;
    std::vector<Bound> overpasses;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;
    std::vector<std::unique_ptr<BridgeSensor>> bridgeSensors;
    std::vector<RoadShape> roadShapes;

    EntityFactory factory(_world, _cfg);

    MapLoader::loadFromYAML(_raceFiles[raceIndex], factory, buildings, bridges,
                            overpasses, checkpoints, spawnPoints, roadShapes,
                            bridgeSensors);

    if (_buildings.empty()) _buildings = std::move(buildings);
    if (_bridges.empty()) _bridges = std::move(bridges);
    if (_overpasses.empty()) _overpasses = std::move(overpasses);

    _players.clear();
    std::vector<Player*> racePlayers;

    for (std::size_t i = 0; i < _playerConfigs.size(); ++i) {
        const auto& pc = _playerConfigs[i];
        if (i >= spawnPoints.size()) break;

        const auto& sp = spawnPoints[i];

        auto car = factory.createCar(pc.carType, sp.x, sp.y, EntityType::Car);
        _world.getCollisionManager().registerCar(car.get(), pc.id);

        auto player = std::make_unique<Player>(pc.id, pc.name, std::move(car));

        player->resetRaceState(0.0f);

        racePlayers.push_back(player.get());
        _players[pc.id] = std::move(player);

        spdlog::info(
            "Jugador {} entra en carrera con carType={} en pos ({}, {})",
            pc.name, static_cast<int>(pc.carType), sp.x, sp.y);
    }

    _traffic = std::make_unique<TrafficSystem>(factory);

    _roadGraph = RoadGraph{};
    for (auto& rs : roadShapes) {
        _roadGraph.addRoad(rs);
    }
    spdlog::debug("[NPC] RoadShapes count = {}", roadShapes.size());
    _roadGraph.build();
    spdlog::debug("[NPC] Graph built: nodes={} segments={}",
                  _roadGraph.getNodes().size(),
                  _roadGraph.getSegments().size());

    _traffic->loadGraph(&_roadGraph);
    for (int i = 0; i < 25; ++i) {
        _traffic->spawnNPCs();
    }
    _race = std::make_unique<RaceSession>(_cfg, std::move(checkpoints),
                                          std::move(spawnPoints), racePlayers);

    _race->setSensors(std::move(bridgeSensors));

    _world.getCollisionManager().setRaceSession(_race.get());
    _race->start();
    spdlog::info("Carrera {} iniciada", _raceFiles[raceIndex]);
}

WorldSnapshot MatchSession::getSnapshot() const {
    WorldSnapshot snap;

    if (!_race) {
        spdlog::error("_race es nullptr en getSnapshot()");
        return snap;
    }

    snap.time = _world.getTime();
    snap.matchState = _state;
    snap.currentRaceIndex = _currentRace;
    snap.raceState = _race->getState();
    snap.raceElapsed = _race->elapsedRaceTime();
    snap.raceCountdown = _race->countdownRemaining();
    snap.raceTimeLeft =
        std::max(0.0f, _cfg.getRaceTimeLimitSec() - _race->elapsedRaceTime());

    for (const auto& player : _players | std::views::values) {
        snap.players.push_back(player->buildSnapshot());
    }
    if (_traffic) {
        for (auto& npc : _traffic->getNPCs()) {
            snap.npcs.push_back(npc->buildSnapshot());
        }
    }
    return snap;
}
CityInfo MatchSession::getCityInfo() const {
    spdlog::debug("getting city info");

    std::vector<Bound> walls;
    std::vector<Bound> railings;
    for (const auto& building : _buildings) {
        const auto [x, y] = building->getPosition();
        Bound bound{Point{x, y}, building->getWidth(), building->getHeight()};
        if (building->getEntityType() == EntityType::Wall)
            walls.push_back(bound);
        if (building->getEntityType() == EntityType::Railing)
            railings.push_back(bound);
    }
    return CityInfo{.name = _raceFiles[_currentRace],
                    .walls = walls,
                    .bridges = _bridges,
                    .railings = railings,
                    .overpasses = _overpasses};
}
RaceInfo MatchSession::getRaceInfo() const {
    std::vector<CheckpointInfo> checkpoints;
    for (const auto& checkpoint : _race->getCheckpoints()) {
        const auto [x, y] = checkpoint->getPosition();
        const float width = checkpoint->getWidth();
        const float height = checkpoint->getHeight();
        checkpoints.push_back(CheckpointInfo{
            .order = static_cast<uint32_t>(checkpoint->getOrder()),
            .bound = {Point{x, y}, width, height},
            .angle = checkpoint->getAngle(),
            .type = checkpoint->getType(),
        });
    }
    spdlog::trace("got checkpoints");

    std::vector<SpawnPointInfo> spawns;
    for (const auto& spawn : _race->getSpawnPoints()) {
        spawns.push_back(SpawnPointInfo{
            .pos = {spawn.x, spawn.y},
            .angle = spawn.angle,
        });
    }
    spdlog::trace("got spawnpoints");
    return RaceInfo{
        .name = _raceFiles[_currentRace],
        .checkpoints = checkpoints,
        .spawnPoints = spawns,
    };
}

void MatchSession::update(float dt) {
    switch (_state) {
        case MatchState::Starting:
            // TODO(elvis): alguna configuracion inicial?
        case MatchState::Racing:

            for (auto& [id, player] : _players) {
                (void)id;
                player->update(dt);
            }
            if (_traffic) _traffic->update(dt);

            _race->update(dt);
            if (_race->isFinished()) {
                pendingEndRacePacket = finishRaceAndComputeTotals();
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

void MatchSession::applyInput(const PlayerId id, const CarInput& car_input) {
    if (_race && _race->getState() != RaceState::Countdown) {
        _players.at(id)->applyInput(car_input);
    }
}
EndRaceSummaryPacket MatchSession::finishRaceAndComputeTotals() {
    auto results = _race->makeResults();
    EndRaceSummaryPacket summary;
    summary.raceIndex = static_cast<uint32_t>(_currentRace);
    for (auto& r : results) {
        auto itPlayer = _players.find(r.id);
        if (itPlayer == _players.end()) continue;

        auto& player = itPlayer->second;
        player->setPenalty(r.penalty);
        player->accumulateTotal();

        EndRaceUpgradeReport rep;
        rep.id = r.id;
        rep.penaltyTime = r.penalty;

        auto itUp = _queuedUpgrades.find(r.id);
        if (itUp != _queuedUpgrades.end()) {
            rep.upgrades = itUp->second;
        }

        summary.results.push_back(rep);
    }
    return summary;
}

void MatchSession::startIntermission() {
    _state = MatchState::Intermission;
    _intermissionClock = 0.0f;
}

void MatchSession::queueUpgrades(
    const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups) {
    _queuedUpgrades = ups;  // se acumulan para la próxima carrera
}

void MatchSession::endIntermissionAndPrepareNextRace() {
    // se aplica upgrades + genera penalizaciones para próxima carrera
    auto penalties = _upgradeSystem.applyForNextRace(_queuedUpgrades);
    for (auto& [pid, ups] : _queuedUpgrades) {
        auto itP = _players.find(pid);
        if (itP == _players.end()) continue;

        auto& player = itP->second;
        player->setUpgrades(ups);
        player->applyUpgrades();
    }

    // se setea penalizaciones iniciales en Player para la próxima carrera
    for (auto& [pid, pen] : penalties) {
        auto itP = _players.find(pid);
        if (itP == _players.end()) continue;
        itP->second->resetRaceState(pen);
    }

    _queuedUpgrades.clear();

    if (_currentRace + 1 < _raceFiles.size()) {
        _state = MatchState::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = MatchState::Finished;
    }
}
