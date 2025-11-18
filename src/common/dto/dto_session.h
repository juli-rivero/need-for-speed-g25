#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_session {
#include "common/dto/macros.inl"

struct StartRequest {
    DECLARE_SERIALIZABLE(StartRequest)
    bool ready;
};

struct StartResponse {
    DECLARE_SERIALIZABLE(StartResponse)
};

struct LeaveRequest {
    DECLARE_SERIALIZABLE(LeaveRequest)
};

struct LeaveResponse {
    DECLARE_SERIALIZABLE(LeaveResponse)
};

struct ChooseCarRequest {
    DECLARE_SERIALIZABLE(ChooseCarRequest)
    std::string car_name;
};

struct SessionSnapshot {
    DECLARE_SERIALIZABLE(SessionSnapshot)

    SessionConfig session;
    std::vector<PlayerInfo> players;
};

#include "common/dto/macros_undef.inl"
}  // namespace dto_session
