#pragma once

#include <memory>
#include <vector>

#include "NPCVehicle.h"
#include "RoadGraph.h"
#include "server/session/logic/types.h"
#include "server/session/physics/EntityFactory.h"

class TrafficSystem {
   public:
    explicit TrafficSystem(EntityFactory& f);

    void loadGraph(RoadGraph* g) { graph = g; }
    void spawnNPCs();
    void update(float dt) const;
    const std::vector<std::unique_ptr<NPCVehicle>>& getNPCs() const;

   private:
    RoadGraph* graph = nullptr;
    EntityFactory& factory;
    std::vector<std::unique_ptr<NPCVehicle>> npcs;
};
