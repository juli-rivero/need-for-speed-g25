#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../session/model/Checkpoint.h"
#include "../session/physics/Box2DPhysicsWorld.h"
#include "server/session/model/Bridge.h"
#include "server/session/model/Wall.h"
#include "yaml-cpp/yaml.h"

class MapLoader {
   public:
    struct MapInfo {
        std::string name;
        std::string city;
        b2Vec2 gravity;
    };

    static MapInfo loadFromYAML(
        const std::string& yamlPath, Box2DPhysicsWorld& world,
        std::vector<std::unique_ptr<Wall>>& walls,
        std::vector<std::unique_ptr<Bridge>>& bridges,
        std::vector<std::unique_ptr<Checkpoint>>& checkpoints,
        std::vector<SpawnPoint>& spawnPoints);
};
