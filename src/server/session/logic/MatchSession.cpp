
#include "MatchSession.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>

#include "../../config/MapLoader.h"
#include "../physics/EntityFactory.h"

MatchSession::MatchSession(const YamlGameConfig& cfg,
                           std::vector<RaceDefinition> raceDefs,
                           Box2DPhysicsWorld& world,
                           std::vector<PlayerConfig> playersCfg)
    : _cfg(cfg),
      _world(world),
      _races(std::move(raceDefs)),
      _playerConfigs(std::move(playersCfg)),
      _upgradeSystem(cfg) {
    if (_races.empty()) {
        _state = MatchState::Finished;
        return;
    }
    _state = MatchState::Racing;
    startRace(0);
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    std::vector<std::unique_ptr<Wall>> Walls;
    std::vector<std::unique_ptr<Bridge>> Bridges;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;

    EntityFactory factory(_world, _cfg);

    MapLoader::loadFromYAML(_races[raceIndex].mapFile, factory, Walls, Bridges,
                            checkpoints, spawnPoints);

    if (_walls.empty()) _walls = std::move(Walls);
    if (_bridges.empty()) _bridges = std::move(Bridges);

    _players.clear();
    std::vector<Player*> racePlayers;

    for (std::size_t i = 0; i < _playerConfigs.size(); ++i) {
        const auto& pc = _playerConfigs[i];
        if (i >= spawnPoints.size()) break;

        const auto& sp = spawnPoints[i];

        auto car = factory.createCar(pc.carType, sp.x, sp.y);
        _world.getCollisionManager().registerCar(car.get(), pc.id);

        auto player = std::make_unique<Player>(pc.id, pc.name, std::move(car));

        player->resetRaceState(0.0f);

        racePlayers.push_back(player.get());
        _players[pc.id] = std::move(player);

        std::cout << " Jugador " << pc.name << " entra en carrera "
                  << " con carType=" << static_cast<int>(pc.carType)
                  << " en pos (" << sp.x << "," << sp.y << ")\n";
    }

    _race = std::make_unique<RaceSession>(_cfg, _races[raceIndex].city,
                                          std::move(checkpoints),
                                          std::move(spawnPoints), racePlayers);

    _world.getCollisionManager().setRaceSession(_race.get());

    _race->start();

    _world.getCollisionManager().setRaceSession(_race.get());

    _race->start();
    std::cout << "Carrera " << (raceIndex + 1) << " iniciada en "
              << _races[raceIndex].city << std::endl;
}

WorldSnapshot MatchSession::getSnapshot() const {
    WorldSnapshot snap;

    if (!_race) {
        std::cerr << "[MatchSession] _race es nullptr en getSnapshot()\n";
        return snap;
    }

    snap.time = _world.getTime();
    const auto& rd = _races[_currentRace];
    snap.raceCity = rd.city;
    snap.raceMapFile = rd.mapFile;
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
    return snap;
}
StaticSnapshot MatchSession::getStaticSnapshot() const {
    spdlog::debug("getting static snapshot");
    StaticSnapshot s;

    const auto& raceDef = _races[_currentRace];
    s.race = raceDef.mapFile;

    for (const auto& w : _walls) {
        WallInfo wi;
        wi.x = w->getPosition().x;
        wi.y = w->getPosition().y;
        wi.w = w->getWidth();
        wi.h = w->getHeight();
        s.walls.push_back(wi);
    }
    for (const auto& br : _bridges) {
        BridgeInfo bi;
        bi.lowerX = br->getLowerPosition().x;
        bi.lowerY = br->getLowerPosition().y;
        bi.upperX = br->getUpperPosition().x;
        bi.upperY = br->getUpperPosition().y;
        bi.w = br->getWidth();
        bi.h = br->getHeight();
        bi.driveable = br->isDriveable();
        s.bridges.push_back(bi);
    }
    for (const auto& cp : _race->getCheckpoints()) {
        CheckpointInfo ci;
        ci.id = cp->getId();
        ci.order = cp->getOrder();
        ci.x = cp->getPosition().x;
        ci.y = cp->getPosition().y;
        ci.w = cp->getWidth();
        ci.h = cp->getHeight();
        s.checkpoints.push_back(ci);
    }
    spdlog::trace("got checkpoints");

    for (size_t i = 0; i < _race->getSpawnPoints().size(); ++i) {
        const auto& sp = _race->getSpawnPoints()[i];
        SpawnPointInfo spi;
        spi.id = static_cast<int>(i);
        spi.x = sp.x;
        spi.y = sp.y;
        spi.angle = sp.angle;
        s.spawns.push_back(spi);
    }
    spdlog::trace("got spawnpoints");

    for (const auto& player : _players | std::views::values) {
        s.players.push_back(player->buildSnapshot());
    }
    spdlog::trace("got players");

    return s;
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

    if (_currentRace + 1 < _races.size()) {
        _state = MatchState::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = MatchState::Finished;
    }
}
