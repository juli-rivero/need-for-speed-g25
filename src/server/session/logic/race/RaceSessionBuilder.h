#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "server/config/YamlGameConfig.h"
#include "server/session/logic/race/RaceSession.h"
#include "server/session/physics/EntityFactory.h"

class RaceSessionBuilder {
    const YamlGameConfig& cfg;
    EntityFactory factory;
    std::vector<Player*> _players;
    std::vector<SpawnPointInfo> _spawnPoints;
    std::vector<std::unique_ptr<Checkpoint>> _checkpoints;

   public:
    RaceSessionBuilder(Box2DPhysicsWorld&, const YamlGameConfig&);
    RaceSessionBuilder& setPlayers(
        const std::unordered_map<PlayerId, std::unique_ptr<Player>>&);
    RaceSessionBuilder& setCheckpoints(const std::vector<CheckpointInfo>&);
    RaceSessionBuilder& setSpawnPoints(const std::vector<SpawnPointInfo>&);
    std::unique_ptr<RaceSession> build();
};
