#pragma once

#include <string>
#include <variant>

#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"
#include "common/protocol.h"
namespace dto {
#include "common/dto/macros.inl"

struct ErrorResponse {
    DECLARE_SERIALIZABLE(ErrorResponse)
    std::string message;
};

enum class RequestType : uint8_t {
    SearchRequest,
    JoinRequest,
    CreateRequest,
    LeaveRequest,
    StartRequest,
};
MAKE_ENUM_SERIALIZABLE(RequestType, uint8_t)

using RequestBody =
    std::variant<dto_search::SearchRequest, dto_search::JoinRequest,
                 dto_search::CreateRequest, dto_session::LeaveRequest,
                 dto_session::StartRequest>;

struct Request {
    // Only sendable, if wanted to be received, it must be done first the type
    // and then a switch for the body
    DECLARE_SENDABLE(Request)
    RequestType type;
    RequestBody body;
};

enum class ResponseType : uint8_t {
    SearchResponse,
    JoinResponse,
    CreateResponse,
    LeaveResponse,
    StartResponse,
    ErrorResponse,
};
MAKE_ENUM_SERIALIZABLE(ResponseType, uint8_t)

using ResponseBody =
    std::variant<dto_search::SearchResponse, dto_search::JoinResponse,
                 dto_search::CreateResponse, dto_session::LeaveResponse,
                 dto_session::StartResponse, ErrorResponse>;

struct Response {
    // Only sendable, if wanted to be received, it must be done first the type
    // and then a switch for the body
    DECLARE_SENDABLE(Response)
    ResponseType type;
    ResponseBody body;
};
#include "common/dto/macros_undef.inl"
}  // namespace dto
