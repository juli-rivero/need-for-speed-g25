#pragma once

#include <memory>
#include <string>

#include "common/structs.h"

enum CollisionCategory : uint32_t {
    CATEGORY_CAR = 0x0001,
    CATEGORY_WALL = 0x0002,
    CATEGORY_RAILING = 0x0004,
    CATEGORY_SENSOR = 0x0008,
    CATEGORY_NPC = 0x0010
};

struct PlayerConfig {
    PlayerId id;       // id único del jugador
    std::string name;  // nombre visible
    CarType carType;   // nombre del tipo de auto (Speedster, Tank, etc.)
};
