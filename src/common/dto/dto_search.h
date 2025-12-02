#pragma once

#include <string>
#include <vector>

#include "common/protocol.h"
#include "common/structs.h"

namespace dto_search {

struct SearchRequest {};

struct SearchResponse {
    std::vector<SessionInfo> sessions;
};

struct JoinRequest {
    std::string session_id;
};

struct JoinResponse {
    SessionInfo session;
    std::vector<CarInfo> carTypes;
};

struct CreateRequest {
    SessionConfig config;
};

struct StaticSessionDataRequest {};

struct StaticSessionDataResponse {
    StaticSessionData data;
};

}  // namespace dto_search

#include "common/dto/macros.inl"
DECLARE_SERIALIZABLE(dto_search::SearchRequest)
DECLARE_SERIALIZABLE(dto_search::SearchResponse)
DECLARE_SERIALIZABLE(dto_search::JoinRequest)
DECLARE_SERIALIZABLE(dto_search::JoinResponse)
DECLARE_SERIALIZABLE(dto_search::CreateRequest)
DECLARE_SERIALIZABLE(dto_search::StaticSessionDataRequest)
DECLARE_SERIALIZABLE(dto_search::StaticSessionDataResponse)
#include "common/dto/macros_undef.inl"
