#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_search {
#include "common/dto/macros.inl"

struct SearchRequest {
    DECLARE_SERIALIZABLE(SearchRequest)
};

struct SearchResponse {
    DECLARE_SERIALIZABLE(SearchResponse)
    std::vector<SessionInfo> sessions;
};

struct JoinRequest {
    DECLARE_SERIALIZABLE(JoinRequest)
    std::string session_id;
};

struct JoinResponse {
    DECLARE_SERIALIZABLE(JoinResponse)
    SessionInfo session;
};

struct CreateRequest {
    DECLARE_SERIALIZABLE(CreateRequest)
    SessionConfig config;
};
struct CreateResponse {
    DECLARE_SERIALIZABLE(CreateResponse)
};

#include "common/dto/macros_undef.inl"
}  // namespace dto_search
