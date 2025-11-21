
#include "MatchSession.h"

#include <algorithm>
#include <iostream>
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
      _upgradeSystem(cfg) {}

void MatchSession::start() {
    if (_races.empty()) {
        _state = MatchState::Finished;
        return;
    }
    _state = MatchState::Racing;
    startRace(0);
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<Bridge>> bridges;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;
    std::vector<std::unique_ptr<BridgeSensor>> bridgeSensors;
    std::vector<PlayerId> playerIds;
    MapLoader::loadFromYAML(_races[raceIndex].mapFile, _world, walls, bridges,
                            checkpoints, spawnPoints, bridgeSensors);

    // Si es la primera carrera, guardá muros/puentes
    if (_walls.empty()) _walls = std::move(walls);
    if (_bridges.empty()) _bridges = std::move(bridges);

    std::vector<std::shared_ptr<Car>> cars;
    _players.clear();

    // se crea los jugadores aqui
    for (size_t i = 0; i < _playerConfigs.size(); ++i) {
        const auto& p = _playerConfigs[i];
        if (i >= spawnPoints.size()) break;
        if (permanentlyDisqualified.count(p.id)) {
            continue;
        }
        const auto& sp = spawnPoints[i];

        // Buscar tipo de auto
        const auto& carTypes = _cfg.getCarTypes();
        const CarType* chosenType = nullptr;
        for (const auto& ct : carTypes) {
            if (ct.name == p.carTypeName) {
                chosenType = &ct;
                break;
            }
        }
        if (!chosenType && !carTypes.empty()) {
            chosenType = &carTypes.front();
        }

        auto car = EntityFactory::createCar(_world, *chosenType, sp.x, sp.y);
        _world.getCollisionManager().registerCar(car.get(), p.id);
        auto player = std::make_unique<Player>(p.id, p.name, std::move(car));
        cars.push_back(player->getCar());
        playerIds.push_back(p.id);
        _players[p.id] = std::move(player);

        std::cout << " Jugador " << p.name << " usa auto tipo '"
                  << chosenType->name << "' en posición (" << sp.x << ","
                  << sp.y << ")\n";
    }

    _race = std::make_unique<RaceSession>(
        _cfg, _races[raceIndex].city, std::move(checkpoints), std::move(cars),
        playerIds, std::move(spawnPoints), _penaltiesForNextRace);
    _world.getCollisionManager().setRaceSession(_race.get());
    _race->start();
    _penaltiesForNextRace.clear();

    std::cout << "Carrera " << (raceIndex + 1) << " iniciada en "
              << _races[raceIndex].city << std::endl;
}

CarSnapshot MatchSession::makeCarSnapshot(const std::shared_ptr<Car>& car) {
    CarSnapshot cs;

    cs.type = YamlGameConfig::getCarSpriteType(car->getType().name);
    cs.x = car->getPosition().x;
    cs.y = car->getPosition().y;
    cs.vx = car->getVelocity().x;
    cs.vy = car->getVelocity().y;
    cs.angle = car->getAngle();
    cs.speed = std::sqrt(cs.vx * cs.vx + cs.vy * cs.vy);
    cs.health = car->getHealth();
    cs.nitroActive = car->isNitroActive();
    cs.accelerating = car->isAccelerating();
    cs.braking = car->isBraking();
    cs.layer = car->getLayer();

    return cs;
}

WorldSnapshot MatchSession::getSnapshot() {
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

    for (const auto& [id, player] : _players) {
        PlayerSnapshot ps;
        ps.id = id;
        ps.name = player->getName();
        ps.car = makeCarSnapshot(player->getCar());
        ps.raceProgress = _race->getProgressForPlayer(id);
        std::cout << "[SnapshotDBG] Player " << id
                  << " nextCP=" << ps.raceProgress.nextCheckpoint << std::endl;
        snap.players.push_back(std::move(ps));
    }
    // snap.collisions = _world.getCollisionManager().consumeEvents();
    snap.permanentlyDQ.assign(permanentlyDisqualified.begin(),
                              permanentlyDisqualified.end());
    return snap;
}
StaticSnapshot MatchSession::getStaticSnapshot() {
    StaticSnapshot s;

    const auto& raceDef = _races[_currentRace];
    s.mapName = raceDef.mapFile;
    s.cityName = raceDef.city;

    for (const auto& w : _walls) {
        WallInfo wi;
        wi.id = w->getId();
        wi.x = w->getPosition().x;
        wi.y = w->getPosition().y;
        wi.w = w->getWidth();
        wi.h = w->getHeight();
        s.walls.push_back(wi);
    }
    for (const auto& br : _bridges) {
        BridgeInfo bi;
        bi.id = br->getId();
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

    for (size_t i = 0; i < _race->getSpawnPoints().size(); ++i) {
        const auto& sp = _race->getSpawnPoints()[i];
        SpawnPointInfo spi;
        spi.id = static_cast<int>(i);
        spi.x = sp.x;
        spi.y = sp.y;
        spi.angle = sp.angle;
        s.spawns.push_back(spi);
    }

    for (const auto& [playerId, player] : _players) {
        const auto& car = player->getCar();
        const auto& type = car->getType();

        // TODO(juli): revisar con elvis, pensé en usar el CarStaticInfo para
        // mandar info inicial para la seleccion de los autos y luego me tope
        // con esto, pero si lo comento no sucede nada
        CarStaticInfo ci = {
            .type = YamlGameConfig::getCarSpriteType(type.name),
            .name = type.name,
            .description = type.description,
            .height = type.height,
            .width = type.width,
            .maxSpeed = type.maxSpeed,
            .acceleration = type.acceleration,
            .mass = type.mass,
            .control = type.control,
            .health = type.maxHealth,
        };
        /*ci.id = playerId;
        ci.playerName = player->getName();
        ci.carType = type.name;
        ci.width = type.width;
        ci.height = type.height;
        ci.maxSpeed = type.maxSpeed;
        ci.acceleration = type.acceleration;
        ci.control = type.control;
        ci.friction = type.friction;
        ci.nitroMultiplier = type.nitroMultiplier;*/
        s.cars.push_back(ci);
    }

    return s;
}
void MatchSession::update(float dt) {
    switch (_state) {
        case MatchState::Starting:
            // TODO(elvis)
        case MatchState::Racing:
            // Actualizar cada coche según su input persistente
            for (auto& [id, player] : _players) {
                auto car = player->getCar();
                car->update(dt);
                if (car->isDestroyed()) {
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
    auto it = _players.find(id);
    if (it == _players.end()) return;
    if (_race && _race->state() != RaceState::Countdown) {
        it->second->getCar()->applyInput(car_input);
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
        auto it = _players.find(pid);
        if (it == _players.end()) continue;

        auto car = it->second->getCar();
        auto& u = car->getUpgrades();
        for (const auto& up : upgrades) {
            switch (up.stat) {
                case UpgradeStat::MaxSpeed:
                    u.bonusMaxSpeed += up.delta;
                    break;

                case UpgradeStat::Acceleration:
                    u.bonusAcceleration += up.delta;
                    break;

                case UpgradeStat::Health:
                    car->increaseMaxHealth(up.delta);
                    break;

                case UpgradeStat::Nitro:
                    u.bonusNitro += up.delta;
                    break;
            }
        }
    }
    _queuedUpgrades.clear();

    if (_currentRace + 1 < _races.size()) {
        _state = MatchState::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = MatchState::Finished;
    }
}
