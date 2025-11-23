#pragma once

#include <memory>
#include <string>
#include <vector>

#include "server/session/model/BridgeSensor.h"
#include "server/session/model/Checkpoint.h"
#include "server/session/model/Wall.h"
#include "server/session/physics/Box2DPhysicsWorld.h"
#include "server/session/physics/EntityFactory.h"
#include "yaml-cpp/yaml.h"

class MapLoader {
   public:
    struct MapInfo {
        std::string name;
        std::string city;
        b2Vec2 gravity;
    };

    static MapInfo loadFromYAML(
        const std::string& yamlPath, EntityFactory& factory,
        std::vector<std::unique_ptr<Wall>>& buildings,
        std::vector<BridgeInfo>& bridges, std::vector<OverpassInfo>& overpasses,
        std::vector<std::unique_ptr<Checkpoint>>& checkpoints,
        std::vector<SpawnPoint>& spawnPoints,
        std::vector<std::unique_ptr<BridgeSensor>>& sensors);
};
