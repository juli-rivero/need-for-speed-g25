
#include "MatchSession.h"

#include <iostream>

#include "../../config/MapLoader.h"      // para cargar CP/Hints desde YAML
#include "../physics/EntityFactory.h"  // para crear entidades (si generás autos aquí)


MatchSession::MatchSession(const YamlGameConfig& cfg,
                           std::vector<RaceDefinition> raceDefs,
                           Box2DPhysicsWorld& world,
                           std::vector<PlayerConfig> players
                           )
    : _cfg(cfg),
      _world(world),
      _players(std::move(players)),
      _races(std::move(raceDefs)),
      _upgradeSystem(cfg) {}

void MatchSession::start() {
    if (_races.empty()) { _state = State::Finished; return; }
    _state = State::Racing;
    startRace(0);
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;


    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<Bridge>> bridges;
    std::vector<Checkpoint> checkpoints;
    std::vector<Hint> hints;
    std::vector<SpawnPoint> spawnPoints;


    MapLoader::loadFromYAML(
        _races[raceIndex].mapFile,
        _world,
        walls,
        bridges,
        checkpoints,
        hints,
        spawnPoints
    );

    // Si es la primera carrera, guardá muros/puentes
    if (_walls.empty()) _walls = std::move(walls);
    if (_bridges.empty()) _bridges = std::move(bridges);


    std::vector<std::shared_ptr<Car>> cars;
    _playerCars.clear();
    // Crear autos según la selección del lobby
    for (size_t i = 0; i < _players.size(); ++i) {
        const auto& p = _players[i];
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
        _playerCars[p.id] = std::shared_ptr<Car>(std::move(car));
        cars.push_back(_playerCars[p.id]);

        std::cout << " Jugador " << p.name << " usa auto tipo '"
                  << chosenType->name << "' en posición (" << sp.x << "," << sp.y << ")\n";
    }

    _race = std::make_unique<RaceSession>(
        _cfg,
        _races[raceIndex].city,
        std::move(checkpoints),
        std::move(hints),
        std::move(cars),
        std::move(spawnPoints),
        _penaltiesForNextRace
    );
    _world.getCollisionManager().setRaceSession(_race.get());
    _race->start();
    _penaltiesForNextRace.clear();

    std::cout << "Carrera " << (raceIndex + 1)
              << " iniciada en " << _races[raceIndex].city << std::endl;
}
WorldSnapshot MatchSession::getSnapshot() {
    if (!_race) {
        std::cerr << "_race es nullptr en getSnapshot()\n";
        return {};
    }
    WorldSnapshot snap;
    snap.time = _world.getTime();  // o reloj interno
    snap.raceTimeLeft = std::max(0.0f, _cfg.getRaceTimeLimitSec() - _race->elapsedRaceTime());
    auto cars = _race->getCars();
    std::cerr << "getCars() devolvió " << cars.size() << " autos\n";
    // recorrer autos de la RaceSession
    for (const auto& car : cars) {
        CarSnapshot cs;
        cs.id = car->getId();
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
        snap.cars.push_back(cs);
    }
    // 🔹 Progreso de cada jugador
    for (const auto& p : _race->getPlayerStates()) {
        RaceProgressSnapshot rp;
        rp.playerId = p.id;
        rp.nextCheckpoint = p.nextCheckpoint;
        rp.finished = p.finished;
        rp.disqualified = p.disqualified;
        rp.elapsedTime = p.elapsed;
        snap.raceProgress.push_back(rp);
    }
    return snap;
}
StaticSnapshot MatchSession::getStaticSnapshot() {
    StaticSnapshot s;

    // 🔹 Datos básicos del mapa
    const auto& raceDef = _races[_currentRace];
    s.mapName  = raceDef.mapFile;
    s.cityName = raceDef.city;

    // 🔹 Extraer walls, bridges, checkpoints, hints y spawnPoints del RaceSession
    //   (estos los cargaste en startRace(), podés guardarlos como miembros)
    for (const auto& w : _walls) {
        WallInfo wi;
        wi.id = w->getId();
        wi.x = w->getPosition().x;
        wi.y = w->getPosition().y;
        wi.w = w->getWidth();
        wi.h = w->getHeight();
        s.walls.push_back(wi);
    }

    for (const auto& cp : _race->getCheckpoints()) {
        CheckpointInfo ci;
        ci.id = cp.getId();
        ci.order = cp.getOrder();
        ci.x = cp.getPosition().x;
        ci.y = cp.getPosition().y;
        ci.w = cp.getWidth();
        ci.h = cp.getHeight();
        s.checkpoints.push_back(ci);
    }

    for (const auto& h : _race->getHints()) {
        HintInfo hi;
        hi.id = h.getId();
        hi.x = h.getPosition().x;
        hi.y = h.getPosition().y;
        s.hints.push_back(hi);
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

    // 🔹 Información estática de cada auto
    for (const auto& [playerId, car] : _playerCars) {
        CarStaticInfo ci;
        ci.id = playerId;
        ci.playerName = "Player_" + std::to_string(playerId); // Podés usar _players[i].name
        ci.carType = car->getType().name; // si tenés getter del tipo
        ci.width  = car->getType().width;
        ci.height = car->getType().height;
        ci.maxSpeed = car->getType().maxSpeed;
        ci.acceleration = car->getType().acceleration;
        ci.control = car->getType().control;
        ci.friction = car->getType().friction;
        ci.nitroMultiplier = car->getType().nitroMultiplier;
        s.cars.push_back(ci);
    }

    return s;
}
void MatchSession::update(float dt) {
    switch (_state) {
        case State::Starting:
            //TODO
        case State::Racing:
            // Actualizar cada coche según su input persistente
            for (auto& [id, car] : _playerCars) {
                car->update();
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

std::shared_ptr<Car> MatchSession::findCarByPlayerId(PlayerId id) {
    auto it = _playerCars.find(id);
    return it != _playerCars.end() ? it->second : nullptr;
}
void MatchSession::applyInput(PlayerId id, const PlayerInput& input) {
    auto car = findCarByPlayerId(id);
    if (!car) return;
    car->setInput(input.accelerate,input.brake,input.turn,input.nitro);
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

void MatchSession::queueUpgrades(const std::unordered_map<PlayerId, std::vector<UpgradeChoice>>& ups) {
    _queuedUpgrades = ups; // se acumulan para la próxima carrera
}

void MatchSession::endIntermissionAndPrepareNextRace() {

    _penaltiesForNextRace = _upgradeSystem.applyForNextRace(_queuedUpgrades);

    // TODO: aplicar mejoras a los autos de cada jugador ANTES de crear la próxima RaceSession.
    // p.ej: playerCar->setMaxSpeed(playerCar->getMaxSpeed() + delta);

    _queuedUpgrades.clear();

    if (_currentRace + 1 < _races.size()) {
        _state = State::Racing;
        startRace(_currentRace + 1);
    } else {
        _state = State::Finished;
    }
}