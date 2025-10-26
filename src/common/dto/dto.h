#pragma once

#include <string>
#include <variant>

#include "common/dto/dto_lobby.h"
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
    LeaveRequest,
    StartRequest,
};
MAKE_ENUM_SERIALIZABLE(RequestType, uint8_t)

using RequestBody =
    std::variant<dto_session::SearchRequest, dto_session::JoinRequest,
                 dto_session::LeaveRequest, dto_lobby::StartRequest>;

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
    LeaveResponse,
    StartResponse,
    ErrorResponse,
};
MAKE_ENUM_SERIALIZABLE(ResponseType, uint8_t)

using ResponseBody =
    std::variant<dto_session::SearchResponse, dto_session::JoinResponse,
                 dto_session::LeaveResponse, dto_lobby::StartResponse,
                 ErrorResponse>;

struct Response {
    // Only sendable, if wanted to be received, it must be done first the type
    // and then a switch for the body
    DECLARE_SENDABLE(Response)
    ResponseType type;
    ResponseBody body;
};
#include "common/dto/macros_undef.inl"
}  // namespace dto
