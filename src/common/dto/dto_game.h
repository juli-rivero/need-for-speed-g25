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

struct GameSnapshotPacket {
    GameSnapshot snapshot;
};

struct EventPacket {
    CollisionEvent collision;
};

struct CheatMessage {
    Cheat type;
};

struct UpgradeRequest {
    UpgradeStat stat;
};

}  // namespace dto_game

#include "common/dto/macros.inl"
DECLARE_SERIALIZABLE(dto_game::GameStaticSnapshot)
DECLARE_SERIALIZABLE(dto_game::TurnRequest)
DECLARE_SERIALIZABLE(dto_game::AccelerateRequest)
DECLARE_SERIALIZABLE(dto_game::UseBoostRequest)
DECLARE_SERIALIZABLE(dto_game::ReverseRequest)
DECLARE_SERIALIZABLE(dto_game::GameSnapshotPacket)
DECLARE_SERIALIZABLE(dto_game::EventPacket)
DECLARE_SERIALIZABLE(dto_game::CheatMessage)
DECLARE_SERIALIZABLE(dto_game::UpgradeRequest)
#include "common/dto/macros_undef.inl"
