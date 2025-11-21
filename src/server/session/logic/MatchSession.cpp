
#include "MatchSession.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>

#include "../../config/MapLoader.h"  // para cargar CP/Hints desde YAML
#include "../physics/EntityFactory.h"  // para crear entidades (si generás autos aquí)

MatchSession::MatchSession(const YamlGameConfig& cfg,
                           std::vector<RaceDefinition> raceDefs,
                           Box2DPhysicsWorld& world,
                           std::vector<PlayerConfig> players)
    : _cfg(cfg),
      _world(world),
      _playerConfigs(players),
      _races(raceDefs),
      _upgradeSystem(cfg) {
    if (_races.empty()) {
        _state = MatchState::Finished;
        return;
    }
    _state = MatchState::Racing;
    startRace(0);
    update(0);
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<Bridge>> bridges;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;
    // std::vector<std::unique_ptr<BridgeSensor>> bridgeSensors;
    std::vector<PlayerId> playerIds;

    EntityFactory factory(_world, _cfg);

    MapLoader::loadFromYAML(_races[raceIndex].mapFile, factory, walls, bridges,
                            checkpoints, spawnPoints);

    // Si es la primera carrera, guardá muros/puentes
    if (_walls.empty()) _walls = std::move(walls);
    if (_bridges.empty()) _bridges = std::move(bridges);

    _players.clear();

    // se crea los jugadores aqui
    for (size_t i = 0; i < _playerConfigs.size(); ++i) {
        const auto& p = _playerConfigs[i];
        if (i >= spawnPoints.size()) break;
        if (permanentlyDisqualified.count(p.id)) {
            continue;
        }
        const auto& sp = spawnPoints[i];

        auto car = factory.createCar(p.carType, sp.x, sp.y);
        _world.getCollisionManager().registerCar(car.get(), p.id);
        playerIds.push_back(p.id);
        _players[p.id] = std::make_unique<Player>(p.id, p.name, std::move(car));

        std::cout << " Jugador " << p.name << " usa auto tipo '"
                  << _cfg.getCarDisplayInfoMap().at(p.carType).name
                  << "' en posición (" << sp.x << "," << sp.y << ")\n";
    }

    _race = std::make_unique<RaceSession>(
        _cfg, _races[raceIndex].city, std::move(checkpoints), playerIds,
        std::move(spawnPoints), _penaltiesForNextRace);

    for (auto& player : _players | std::views::values)
        player->setRace(_race.get());

    _world.getCollisionManager().setRaceSession(_race.get());
    _race->start();
    _penaltiesForNextRace.clear();

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
    snap.raceState = _race->state();
    snap.raceElapsed = _race->elapsedRaceTime();
    snap.raceCountdown = _race->countdownRemaining();
    snap.raceTimeLeft =
        std::max(0.0f, _cfg.getRaceTimeLimitSec() - _race->elapsedRaceTime());

    for (const auto& player : _players | std::views::values) {
        snap.players.push_back(player->get_snapshot());
    }
    // snap.collisions = _world.getCollisionManager().consumeEvents();
    snap.permanentlyDQ.assign(permanentlyDisqualified.begin(),
                              permanentlyDisqualified.end());
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
        s.players.push_back(player->get_snapshot());
    }
    spdlog::trace("got players");

    return s;
}
void MatchSession::update(float dt) {
    switch (_state) {
        case MatchState::Starting:
            // TODO(elvis)
        case MatchState::Racing:
            // Actualizar cada coche según su input persistente
            for (auto& [id, player] : _players) {
                player->update(dt);
                if (player->idDead()) {
                    _race->onCarDestroyed(id);
                    permanentlyDisqualified.insert(id);
                }
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
    if (_race && _race->state() != RaceState::Countdown) {
        _players.at(id)->applyInput(car_input);
    }
}
EndRaceSummaryPacket MatchSession::finishRaceAndComputeTotals() {
    _lastResults = _race->makeResults();
    for (const auto& r : _lastResults) {
        if (!r.dnf) {
            _totalTime[r.id] += r.netTime;  // acumulado por partida
        }
    }
    // ← crear paquete para el cliente
    EndRaceSummaryPacket summary;
    summary.raceIndex = _currentRace;
    for (const auto& r : _lastResults) {
        EndRaceUpgradeReport rep;
        rep.id = r.id;
        rep.penaltyTime = r.penalty;
        auto it = _queuedUpgrades.find(r.id);
        if (it != _queuedUpgrades.end()) rep.upgrades = it->second;

        summary.results.push_back(rep);
    }

    return summary;  // ← antes no retornabas nada!
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
    _penaltiesForNextRace = _upgradeSystem.applyForNextRace(_queuedUpgrades);

    for (auto& [pid, upgrades] : _queuedUpgrades) {
        auto& player = _players.at(pid);
        for (const auto& up : upgrades) player->upgradeCar(up);
    }
    _queuedUpgrades.clear();

    if (_currentRace + 1 < _races.size()) {
        _state = MatchState::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = MatchState::Finished;
    }
}
