#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"

namespace dto_session {
#include "common/dto/macros.inl"

struct SearchRequest {
    DECLARE_SERIALIZABLE(SearchRequest)
};

struct SearchResponse {
    DECLARE_SERIALIZABLE(SearchResponse)
    struct SessionInfo {
        std::string session_id;
        uint16_t players_count;
    };
    std::vector<SessionInfo> sessions;
};

struct JoinRequest {
    DECLARE_SERIALIZABLE(JoinRequest)
    std::string session_id;
};

struct JoinResponse {
    DECLARE_SERIALIZABLE(JoinResponse)
};

struct LeaveRequest {
    DECLARE_SERIALIZABLE(LeaveRequest)
};

struct LeaveResponse {
    DECLARE_SERIALIZABLE(LeaveResponse)
};
#include "common/dto/macros_undef.inl"
}  // namespace dto_session
