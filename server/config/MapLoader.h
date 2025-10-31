
#ifndef TALLER_TP_MAPLOADER_H
#define TALLER_TP_MAPLOADER_H


#include <string>
#include <vector>
#include <memory>
#include "yaml-cpp/yaml.h"
#include "../physics/Box2DPhysicsWorld.h"
#include "../model/Checkpoint.h"
#include "../model/Hint.h"


class Wall;
class Bridge;

struct SpawnPoint {
    float x, y, angle;
};

struct PlayerConfig {
    int id;
    std::string name;
    std::string color;
    float maxSpeed, acceleration, health, control;
};

class MapLoader {
public:
    struct MapInfo {
        std::string name;
        std::string city;
        b2Vec2 gravity;
    };

    static MapInfo loadFromYAML(
        const std::string& yamlPath,
        Box2DPhysicsWorld& world,
        std::vector<std::unique_ptr<Wall>>& walls,
        std::vector<std::unique_ptr<Bridge>>& bridges,
        std::vector<Checkpoint>& checkpoints,
        std::vector<Hint>& hints,
        std::vector<SpawnPoint>& spawnPoints

    );
};


#endif
