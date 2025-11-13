
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
      _playerConfigs(std::move(players)),
      _races(std::move(raceDefs)),
      _upgradeSystem(cfg) {}

void MatchSession::start() {
    if (_races.empty()) {
        _state = State::Finished;
        return;
    }
    _state = State::Racing;
    startRace(0);
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<Bridge>> bridges;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;
    std::vector<PlayerId> playerIds;
    MapLoader::loadFromYAML(_races[raceIndex].mapFile, _world, walls, bridges,
                            checkpoints, spawnPoints);

    // Si es la primera carrera, guardá muros/puentes
    if (_walls.empty()) _walls = std::move(walls);
    if (_bridges.empty()) _bridges = std::move(bridges);

    std::vector<std::shared_ptr<Car>> cars;
    _players.clear();

    // se crea los jugadores aqui
    for (size_t i = 0; i < _playerConfigs.size(); ++i) {
        const auto& p = _playerConfigs[i];
        if (i >= spawnPoints.size()) break;
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

static CarSpriteType toCarSpriteType(const std::string& name) {
    if (name == "Speedster") return CarSpriteType::Speedster;
    if (name == "Muscle") return CarSpriteType::Muscle;
    if (name == "Offroad") return CarSpriteType::Offroad;
    return CarSpriteType::Truck;  // valor por defecto
}

CarSnapshot MatchSession::makeCarSnapshot(const std::shared_ptr<Car>& car) {
    CarSnapshot cs;

    cs.type = toCarSpriteType(car->getType().name);
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

    return cs;
}

WorldSnapshot MatchSession::getSnapshot() {
    WorldSnapshot snap;

    if (!_race) {
        std::cerr << "[MatchSession] _race es nullptr en getSnapshot()\n";
        return snap;
    }

    snap.time = _world.getTime();
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

        CarStaticInfo ci;
        ci.id = playerId;
        ci.playerName = player->getName();
        ci.carType = type.name;
        ci.width = type.width;
        ci.height = type.height;
        ci.maxSpeed = type.maxSpeed;
        ci.acceleration = type.acceleration;
        ci.control = type.control;
        ci.friction = type.friction;
        ci.nitroMultiplier = type.nitroMultiplier;
        s.cars.push_back(ci);
    }

    return s;
}
void MatchSession::update(float dt) {
    switch (_state) {
        case State::Starting:
            // TODO(elvis)
        case State::Racing:
            // Actualizar cada coche según su input persistente
            for (auto& [id, player] : _players) {
                player->getCar()->update();
            }
            _race->update(dt);
            if (_race->isFinished()) {
                finishRaceAndComputeTotals();
                startIntermission();
            }
            break;
        case State::Intermission:
            _intermissionClock += dt;
            if (_intermissionClock >= _cfg.getIntermissionSec()) {
                endIntermissionAndPrepareNextRace();
            }
            break;
        case State::Finished:
            break;
    }
}

void MatchSession::applyInput(PlayerId id, const PlayerInput& input) {
    auto it = _players.find(id);
    if (it == _players.end()) return;
    it->second->getCar()->setInput(input.accelerate, input.brake, input.turn,
                                   input.nitro);
}
void MatchSession::finishRaceAndComputeTotals() {
    _lastResults = _race->makeResults();
    for (const auto& r : _lastResults) {
        if (!r.dnf) {
            _totalTime[r.id] += r.netTime;  // acumulado por partida
        }
    }
}

void MatchSession::startIntermission() {
    _state = State::Intermission;
    _intermissionClock = 0.0f;
}

void MatchSession::queueUpgrades(
    const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups) {
    _queuedUpgrades = ups;  // se acumulan para la próxima carrera
}

void MatchSession::endIntermissionAndPrepareNextRace() {
    _penaltiesForNextRace = _upgradeSystem.applyForNextRace(_queuedUpgrades);

    // TODO(elvis): aplicar mejoras a los autos de cada jugador ANTES de crear
    // la próxima RaceSession. p.ej:
    // playerCar->setMaxSpeed(playerCar->getMaxSpeed() + delta);

    _queuedUpgrades.clear();

    if (_currentRace + 1 < _races.size()) {
        _state = State::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = State::Finished;
    }
}
