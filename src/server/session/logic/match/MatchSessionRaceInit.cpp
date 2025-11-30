#include "MatchSession.h"
#include "server/config/MapLoader.h"

static void loadMapData(const std::string& file, EntityFactory& factory,
                        std::vector<std::unique_ptr<Wall>>& buildings,
                        std::vector<Bound>& bridges,
                        std::vector<Bound>& overpasses,
                        std::vector<std::unique_ptr<Checkpoint>>& checkpoints,
                        std::vector<SpawnPoint>& spawnPoints,
                        std::vector<RoadShape>& roadShapes,
                        std::vector<std::unique_ptr<BridgeSensor>>& sensors) {
    MapLoader::loadFromYAML(file, factory, buildings, bridges, overpasses,
                            checkpoints, spawnPoints, roadShapes, sensors);
}

static void attachPlayers(
    Box2DPhysicsWorld& world, EntityFactory& factory,
    const std::vector<PlayerConfig>& configs,
    const std::vector<SpawnPoint>& spawns,
    std::unordered_map<PlayerId, std::unique_ptr<Player>>& outPlayers,
    std::vector<Player*>& racePlayers) {
    outPlayers.clear();
    racePlayers.clear();

    for (size_t i = 0; i < configs.size() && i < spawns.size(); ++i) {
        const auto& pc = configs[i];
        const auto& sp = spawns[i];

        auto car = factory.createCar(pc.carType, sp.x, sp.y, sp.angle,
                                     EntityType::Car);
        world.getCollisionManager().registerCar(car.get(), pc.id);

        auto player = std::make_unique<Player>(pc.id, pc.name, std::move(car));
        player->resetRaceState(0);

        racePlayers.push_back(player.get());
        outPlayers[pc.id] = std::move(player);
    }
}

static void initRoadGraph(RoadGraph& graph,
                          const std::vector<RoadShape>& shapes) {
    for (auto& r : shapes) graph.addRoad(r);
    graph.build();
}
static void spawnNPCs(TrafficSystem& traffic, int count) {
    for (int i = 0; i < count; ++i) traffic.spawnNPCs();
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    std::vector<std::unique_ptr<Wall>> buildings;
    std::vector<Bound> bridges, overpasses;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<SpawnPoint> spawnPoints;
    std::vector<std::unique_ptr<BridgeSensor>> sensors;
    std::vector<RoadShape> roadShapes;

    EntityFactory factory(_world, _cfg);

    // se carga el mapa
    loadMapData(_raceFiles[raceIndex], factory, buildings, bridges, overpasses,
                checkpoints, spawnPoints, roadShapes, sensors);

    // se copia edificios fijos solo una vez
    if (_buildings.empty()) _buildings = std::move(buildings);
    if (_bridges.empty()) _bridges = std::move(bridges);
    if (_overpasses.empty()) _overpasses = std::move(overpasses);

    // se crea jugadores
    std::vector<Player*> racePlayers;
    attachPlayers(_world, factory, _playerConfigs, spawnPoints, _players,
                  racePlayers);

    //  sistema NPC
    _traffic = std::make_unique<TrafficSystem>(factory, spawnPoints);
    initRoadGraph(_roadGraph, roadShapes);
    _traffic->loadGraph(&_roadGraph);
    spawnNPCs(*_traffic, _cfg.getMaxNPCS());

    // se crea RaceSession
    _race = std::make_unique<RaceSession>(_cfg, std::move(checkpoints),
                                          std::move(spawnPoints), racePlayers);

    _race->setSensors(std::move(sensors));
    _world.getCollisionManager().setRaceSession(_race.get());
    _race->start();
}
