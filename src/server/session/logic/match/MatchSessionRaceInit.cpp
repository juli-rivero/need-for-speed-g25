#include "MatchSession.h"
#include "server/config/MapLoader.h"
#include "server/session/logic/race/RaceSessionBuilder.h"

static void spawnNPCs(TrafficSystem& traffic, int count) {
    for (int i = 0; i < count; ++i) traffic.spawnNPCs();
}

void MatchSession::startRace(std::size_t raceIndex) {
    _currentRace = raceIndex;

    // se carga el mapa
    const MapLoader loader(_raceFiles[raceIndex], _cfg);

    const auto& checkpoints = loader.getCheckpoints();
    const auto& spawnPoints = loader.getSpawnPoints();

    RaceSessionBuilder raceBuilder(_world, _cfg);
    raceBuilder.setPlayers(_players)
        .setCheckpoints(checkpoints)
        .setSpawnPoints(spawnPoints);
    _race = raceBuilder.build();

    //  sistema NPC
    _traffic =
        std::make_unique<TrafficSystem>(_world, _cfg, _roadGraph, spawnPoints);

    spawnNPCs(*_traffic, _cfg.getMaxNPCS());

    _world.getCollisionManager().setRaceSession(_race.get());
    _race->start();
}
