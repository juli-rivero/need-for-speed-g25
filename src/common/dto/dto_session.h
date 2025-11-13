#pragma once

#include "common/protocol.h"

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

#include "common/dto/macros_undef.inl"
}  // namespace dto_session
