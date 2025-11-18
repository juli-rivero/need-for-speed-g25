#include "common/dto/dto.h"

#include <iostream>

// ErrorResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto::ErrorResponse& e) {
    return p >> e.message;
}
ProtocolSender& operator<<(ProtocolSender& p, const dto::ErrorResponse& e) {
    return p << e.message;
}

using dto::Request;
// Request SENDABLE
ProtocolSender& operator<<(ProtocolSender& p, const Request& e) {
    p << e.index();
    std::visit([&](const auto& val) { p << val; }, e);
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, Request& e) {
    /*
    * std::variant<dto_search::SearchRequest, dto_search::JoinRequest,
                 dto_search::CreateRequest, dto_session::LeaveRequest,
                 dto_session::StartRequest, dto_session::ChooseCarRequest,
                 dto_game::TurnRequest, dto_game::AccelerateRequest,
                 dto_game::UseBoostRequest>;
     */
    switch (p.get<size_t>()) {
        case 0:
            e = p.get<dto_search::SearchRequest>();
            break;
        case 1:
            e = p.get<dto_search::JoinRequest>();
            break;
        case 2:
            e = p.get<dto_search::CreateRequest>();
            break;
        case 3:
            e = p.get<dto_session::LeaveRequest>();
            break;
        case 4:
            e = p.get<dto_session::StartRequest>();
            break;
        case 5:
            e = p.get<dto_session::ChooseCarRequest>();
            break;
        case 6:
            e = p.get<dto_game::GameInfoRequest>();
            break;
        case 7:
            e = p.get<dto_game::TurnRequest>();
            break;
        case 8:
            e = p.get<dto_game::AccelerateRequest>();
            break;
        case 9:
            e = p.get<dto_game::UseBoostRequest>();
            break;
        case 10:
            e = p.get<dto_game::ReverseRequest>();
            break;
        default:
            throw std::runtime_error("Unknown request type");
    }
    return p;
}

using dto::Response;
// Response SENDABLE
ProtocolSender& operator<<(ProtocolSender& p, const Response& e) {
    p << e.index();
    std::visit([&](const auto& val) { p << val; }, e);
    return p;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, Response& e) {
    /*
    * std::variant<dto_search::SearchResponse, dto_search::JoinResponse,
                 dto_session::LeaveResponse, dto_session::StartResponse,
                 ErrorResponse, dto_session::SessionSnapshot,
                 dto_game::SnapshotResponse>;
     */
    switch (p.get<size_t>()) {
        case 0:
            e = p.get<dto_search::SearchResponse>();
            break;
        case 1:
            e = p.get<dto_search::JoinResponse>();
            break;
        case 2:
            e = p.get<dto_session::LeaveResponse>();
            break;
        case 3:
            e = p.get<dto_session::StartResponse>();
            break;
        case 4:
            e = p.get<dto::ErrorResponse>();
            break;
        case 5:
            e = p.get<dto_session::SessionSnapshot>();
            break;
        case 6:
            e = p.get<dto_game::GameInfoResponse>();
            break;
        case 7:
            e = p.get<dto_game::GameStaticSnapshot>();
            break;
        case 8:
            e = p.get<dto_game::GameSnapshot>();
            break;
        default:
            throw std::runtime_error("Unknown request type");
    }
    return p;
}
