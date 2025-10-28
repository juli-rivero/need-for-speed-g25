
#ifndef TALLER_TP_MAPLOADER_H
#define TALLER_TP_MAPLOADER_H


#include <string>
#include <vector>
#include <memory>
#include "yaml-cpp/yaml.h"
#include "../physics/PhysicsWorld.h"
#include "../model/Checkpoint.h"
#include "../model/Hint.h"

#include "../model/Car.h"

class Wall;
class Bridge;

struct SpawnPoint {
    int playerId;
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
    static void loadFromYAML(
        const std::string& path,
        PhysicsWorld& world,
        std::size_t raceIndex,
        std::vector<std::unique_ptr<Wall>>& walls,
        std::vector<std::unique_ptr<Bridge>>& bridges,
        std::vector<Checkpoint>& checkpoints,
        std::vector<Hint>& hints,
        std::vector<SpawnPoint>& spawnPoints,
        std::vector<PlayerConfig>& players
    );
};

#endif
