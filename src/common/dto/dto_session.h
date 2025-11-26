#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_session {

struct StartRequest {
    bool ready;
};

struct StartResponse {
    CityInfo city_info;
    RaceInfo first_race_info;
};

struct LeaveRequest {};

struct LeaveResponse {};

struct ChooseCarRequest {
    CarType car_type;
};

struct SessionSnapshot {
    SessionConfig session;
    std::vector<PlayerInfo> players;
};

}  // namespace dto_session

#include "common/dto/macros.inl"
DECLARE_SERIALIZABLE(dto_session::StartRequest)
DECLARE_SERIALIZABLE(dto_session::StartResponse)
DECLARE_SERIALIZABLE(dto_session::LeaveRequest)
DECLARE_SERIALIZABLE(dto_session::LeaveResponse)
DECLARE_SERIALIZABLE(dto_session::ChooseCarRequest)
DECLARE_SERIALIZABLE(dto_session::SessionSnapshot)
#include "common/dto/macros_undef.inl"
