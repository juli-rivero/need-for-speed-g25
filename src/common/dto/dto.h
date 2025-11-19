#pragma once

#include <string>
#include <variant>

#include "common/dto/dto_game.h"
#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"
#include "common/protocol.h"
namespace dto {
#include "common/dto/macros.inl"

struct ErrorResponse {
    DECLARE_SERIALIZABLE(ErrorResponse)
    std::string message;
};

using Request =
    std::variant<dto_search::SearchRequest, dto_search::JoinRequest,
                 dto_search::CreateRequest, dto_session::LeaveRequest,
                 dto_session::StartRequest, dto_session::ChooseCarRequest,
                 dto_game::TurnRequest, dto_game::AccelerateRequest,
                 dto_game::UseBoostRequest, dto_game::ReverseRequest>;

using Response =
    std::variant<dto_search::SearchResponse, dto_search::JoinResponse,
                 dto_session::LeaveResponse, dto_session::StartResponse,
                 ErrorResponse, dto_session::SessionSnapshot,
                 dto_game::GameSnapshot>;

#include "common/dto/macros_undef.inl"
}  // namespace dto

ProtocolSender& operator<<(ProtocolSender& p, const dto::Request&);
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto::Request&);
ProtocolSender& operator<<(ProtocolSender& p, const dto::Response&);
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto::Response&);
