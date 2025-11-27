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

struct BridgeInfo {
    int id;
    float lowerX, lowerY;
    float upperX, upperY;
    float w, h;
    bool driveable;
};

/*
// snapshot estático (solo al empezar la partida)
struct StaticSnapshot {
    std::string mapName;
    std::string cityName;

    std::vector<WallInfo> walls;
    std::vector<BridgeInfo> bridges;
    std::vector<CheckpointInfo> checkpoints;
    std::vector<SpawnPointInfo> spawns;
    std::vector<CarStaticInfo> cars;
};
*/

struct WorldSnapshot {
    float time{0.0f};  // tiempo global simulado
    std::string raceCity;
    std::string raceMapFile;

    // estado de MatchSession
    MatchState matchState{MatchState::Starting};
    uint32_t currentRaceIndex{0};

    // estado de RaceSession
    RaceState raceState{RaceState::Countdown};
    float raceElapsed{0.0f};
    float raceCountdown{0.0f};

    float raceTimeLeft{0.0f};  // tiempo restante si hay límite (10min)
    std::vector<PlayerSnapshot> players;
    std::vector<PlayerId> permanentlyDQ;
};
