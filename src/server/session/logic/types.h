#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/structs.h"

enum CollisionCategory : uint32_t {
    CATEGORY_CAR = 0x0001,
    CATEGORY_WALL = 0x0002,
    CATEGORY_RAILING = 0x0004,
    CATEGORY_SENSOR = 0x0008,
    CATEGORY_NPC = 0x0010
};

struct SpawnPoint {
    float x, y, angle;
};
struct PlayerConfig {
    PlayerId id;       // id único del jugador
    std::string name;  // nombre visible
    CarType carType;   // nombre del tipo de auto (Speedster, Tank, etc.)
};
// esto estaba en racesession, lo muevo aca
struct PlayerRaceData {
    PlayerId id{};
    std::size_t nextCheckpoint{0};
    bool finished{false};
    bool disqualified{false};
    float elapsed{0.0f};      // tiempo crudo
    float penaltyTime{0.0f};  // penalización a aplicar en esta carrera
};

enum class UpgradeStat { Acceleration, MaxSpeed, Nitro, Health };
struct UpgradeChoice {
    UpgradeStat stat;
    float delta;    // cuánto aumenta el stat
    float penalty;  // penalidad por aplicar este upgrade
};
