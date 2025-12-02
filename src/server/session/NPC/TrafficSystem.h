#pragma once

#include <memory>
#include <vector>

#include "NPCVehicle.h"
#include "RoadGraph.h"
#include "server/config/YamlGameConfig.h"
#include "server/session/physics/EntityFactory.h"

class TrafficSystem {
   public:
    explicit TrafficSystem(
        Box2DPhysicsWorld& world, const YamlGameConfig& cfg,
        const RoadGraph& graph,
        const std::vector<SpawnPointInfo>& playerSpawnPoints);
    void spawnNPCs();
    void update(float dt) const;
    const std::vector<std::unique_ptr<NPCVehicle>>& getNPCs() const;

   private:
    const RoadGraph& graph;
    EntityFactory factory;
    std::vector<SpawnPointInfo> playerSpawnPoints;
    std::vector<std::unique_ptr<NPCVehicle>> npcs;

    bool isTooCloseToSpawnPoints(const Point& pos) const;
};
