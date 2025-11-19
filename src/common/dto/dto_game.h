#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_game {
struct GameStaticSnapshot {
    StaticSnapshot info;
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

}  // namespace dto_game

#include "common/dto/macros.inl"
DECLARE_SERIALIZABLE(dto_game::GameStaticSnapshot)
DECLARE_SERIALIZABLE(dto_game::TurnRequest)
DECLARE_SERIALIZABLE(dto_game::AccelerateRequest)
DECLARE_SERIALIZABLE(dto_game::UseBoostRequest)
DECLARE_SERIALIZABLE(dto_game::ReverseRequest)
DECLARE_SERIALIZABLE(dto_game::GameSnapshot)
#include "common/dto/macros_undef.inl"
