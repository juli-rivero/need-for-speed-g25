
#include "MatchSession.h"

#include <iostream>

#include "../config/MapLoader.h"      // para cargar CP/Hints desde YAML
#include "../physics/EntityFactory.h"  // para crear entidades (si generás autos aquí)
#include "../physics/Box2DBodyAdapter.h"

MatchSession::MatchSession(const IGameConfig& cfg,
                           std::vector<RaceDefinition> raceDefs,
                           PhysicsWorld& world)
    : _cfg(cfg),
      _world(world),
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
    std::vector<PlayerConfig> playerConfigs;

    MapLoader::loadFromYAML(
        _races[raceIndex].mapFile,
        _world,
        raceIndex,
        walls,
        bridges,
        checkpoints,
        hints,
        spawnPoints,
        playerConfigs
    );


    std::vector<std::shared_ptr<Car>> cars;
    auto worldId = _world.getWorldId();

    for (size_t i = 0; i < playerConfigs.size(); ++i) {
        const auto& p = playerConfigs[i];
        if (i >= spawnPoints.size()) break;
        const auto& sp = spawnPoints[i];

        auto car = EntityFactory::createCar(worldId, p.id, p.color, sp.x, sp.y);
        cars.push_back(std::move(car));
    }

    _race = std::make_unique<RaceSession>(
        _cfg,
        _races[raceIndex].city,
        std::move(checkpoints),
        std::move(hints),
        std::move(cars),
        _penaltiesForNextRace
    );

    _race->start();
    _penaltiesForNextRace.clear();

    std::cout << "Carrera " << (raceIndex + 1)
              << " iniciada en " << _races[raceIndex].mapFile << std::endl;
}

void MatchSession::update(float dt) {
    switch (_state) {
    case State::Lobby:
        // TODO: esperar a jugadores listos; cuando estén ,start()
        break;
    case State::Racing:
        _race->update(dt);
        if (_race->isFinished()) {
            finishRaceAndComputeTotals();
            startIntermission();
        }
        break;
    case State::Intermission:
        _intermissionClock += dt;
        if (_intermissionClock >= _cfg.intermissionSec()) {
            endIntermissionAndPrepareNextRace();
        }
        break;
    case State::Finished:
        // nada
        break;
    }
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