#pragma once

#include <string>
#include <vector>

#include "common/structs.h"

struct PlayerInput {
    bool accelerate{false};
    bool brake{false};
    bool nitro{false};
    TurnDirection turn{TurnDirection::None};
};

struct WallInfo {
    int id;
    float x, y;
    float w, h;
};

struct CheckpointInfo {
    int id;
    int order;
    float x, y;
    float w;
    float h;
};

struct HintInfo {
    int id;
    float x, y;
};

struct SpawnPointInfo {
    int id;
    float x, y;
    float angle;
};

struct BridgeInfo {
    int id;
    float lowerX, lowerY;
    float upperX, upperY;
    float w, h;
    bool driveable;
};

// snapshot estático (solo al empezar la partida)
struct StaticSnapshot {
    std::string mapName;
    std::string cityName;

    std::vector<WallInfo> walls;
    std::vector<BridgeInfo> bridges;
    std::vector<CheckpointInfo> checkpoints;
    std::vector<HintInfo> hints;
    std::vector<SpawnPointInfo> spawns;
    std::vector<CarStaticInfo> cars;
};
