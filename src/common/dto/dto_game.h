#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_game {
struct GameStaticSnapshot {
    RaceInfo new_race_info;
};

struct TurnRequest {
    TurnDirection direction;
    bool turn;
};
struct AccelerateRequest {
    bool accelerate;
};

struct UseBoostRequest {
    bool useBoost;
};

struct ReverseRequest {
    bool reverse;
};

struct GameSnapshot {
    float raceTimeLeft;
    std::vector<PlayerSnapshot> players;
};

struct EventPacket {
    CollisionEvent collision;
};

}  // namespace dto_game

#include "common/dto/macros.inl"
DECLARE_SERIALIZABLE(dto_game::GameStaticSnapshot)
DECLARE_SERIALIZABLE(dto_game::TurnRequest)
DECLARE_SERIALIZABLE(dto_game::AccelerateRequest)
DECLARE_SERIALIZABLE(dto_game::UseBoostRequest)
DECLARE_SERIALIZABLE(dto_game::ReverseRequest)
DECLARE_SERIALIZABLE(dto_game::GameSnapshot)
DECLARE_SERIALIZABLE(dto_game::EventPacket)
#include "common/dto/macros_undef.inl"
