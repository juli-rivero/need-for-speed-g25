#pragma once

#include "common/dto/macros.inl"
#include "common/protocol.h"
#include "common/structs.h"

MAKE_ENUM_SERIALIZABLE(SessionStatus, uint8_t)
DECLARE_SERIALIZABLE(SessionConfig)
DECLARE_SERIALIZABLE(SessionInfo)
MAKE_ENUM_SERIALIZABLE(CarType, uint8_t)
DECLARE_SERIALIZABLE(CarDisplayInfo)
DECLARE_SERIALIZABLE(PlayerInfo)
DECLARE_SERIALIZABLE(CarStaticStats)
DECLARE_SERIALIZABLE(CarInfo)
MAKE_ENUM_SERIALIZABLE(TurnDirection, uint8_t)
DECLARE_SERIALIZABLE(CollisionSimple)
DECLARE_SERIALIZABLE(CollisionCarToCar)
DECLARE_SERIALIZABLE(CollisionEvent)
MAKE_ENUM_SERIALIZABLE(MatchState, uint8_t)
MAKE_ENUM_SERIALIZABLE(RaceState, uint8_t)
DECLARE_SERIALIZABLE(CarSnapshot)
DECLARE_SERIALIZABLE(RaceProgressSnapshot)
DECLARE_SERIALIZABLE(PlayerSnapshot)
DECLARE_SERIALIZABLE(SpawnPointInfo)
DECLARE_SERIALIZABLE(CheckpointInfo)
DECLARE_SERIALIZABLE(WallInfo)
DECLARE_SERIALIZABLE(BridgeInfo)
DECLARE_SERIALIZABLE(StaticSnapshot)

#include "common/dto/macros_undef.inl"
