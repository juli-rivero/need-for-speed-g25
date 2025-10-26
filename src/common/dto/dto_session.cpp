#include "common/dto/dto_session.h"

namespace dto_session {

// SearchRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_session::SearchRequest&) {
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_session::SearchRequest&) {
    return p;
}
// SearchResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p,
                             dto_session::SearchResponse& e) {
    e.sessions.resize(p.get<uint16_t>());
    for (auto& session : e.sessions) {
        p >> session.players_count >> session.session_id;
    }
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_session::SearchResponse& e) {
    p << static_cast<uint16_t>(e.sessions.size());
    for (const auto& session : e.sessions) {
        p << session.players_count << session.session_id;
    }
    return p;
}
// JoinRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_session::JoinRequest& e) {
    p >> e.session_id;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_session::JoinRequest& e) {
    p << e.session_id;
    return p;
}
// JoinResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_session::JoinResponse&) {
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_session::JoinResponse&) {
    return p;
}
// LeaveRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_session::LeaveRequest&) {
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_session::LeaveRequest&) {
    return p;
}
// LeaveResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_session::LeaveResponse&) {
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_session::LeaveResponse&) {
    return p;
}
}  // namespace dto_session
