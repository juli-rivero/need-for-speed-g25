#pragma once
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_game {
#include "common/dto/macros.inl"

struct TurnRequest {
    DECLARE_SERIALIZABLE(TurnRequest)
    TurnDirection direction;
    bool turn;
};
struct AccelerateRequest {
    DECLARE_SERIALIZABLE(AccelerateRequest)
    bool accelerate;
};

struct UseBoostRequest {
    DECLARE_SERIALIZABLE(UseBoostRequest)
    bool useBoost;
};

struct ReverseRequest {
    DECLARE_SERIALIZABLE(ReverseRequest)
    bool reverse;
};

struct GameSnapshot {
    DECLARE_SERIALIZABLE(GameSnapshot)
    float raceTimeLeft;
    std::vector<PlayerSnapshot> players;
};

#include "common/dto/macros_undef.inl"
}  // namespace dto_game
