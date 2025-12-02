#pragma once

#include <string>
#include <variant>

#include "common/dto/dto_game.h"
#include "common/dto/dto_search.h"
#include "common/dto/dto_session.h"
#include "common/protocol.h"
namespace dto {

struct ErrorResponse {
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
                 dto_game::GameStaticSnapshot, dto_game::GameSnapshotPacket,
                 dto_game::EventPacket>;

}  // namespace dto

#include "common/dto/macros.inl"
DECLARE_SERIALIZABLE(dto::ErrorResponse)
DECLARE_SERIALIZABLE(dto::Request)
DECLARE_SERIALIZABLE(dto::Response)
#include "common/dto/macros_undef.inl"
