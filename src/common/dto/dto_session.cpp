#include "common/dto/dto_session.h"

namespace dto_session {

// SearchRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SearchRequest&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const SearchRequest&) {
    return p;
}
// SearchResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SearchResponse& e) {
    e.sessions.resize(p.get<size_t>());
    for (auto& session : e.sessions) {
        p >> session.city >> session.maxPlayers >> session.name >>
            session.raceCount >> session.currentPlayers;
        session.status = static_cast<SessionStatus>(p.get<uint8_t>());
    }
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const SearchResponse& e) {
    p << e.sessions.size();
    for (const auto& session : e.sessions) {
        p << session.city << session.maxPlayers << session.name
          << session.raceCount << session.currentPlayers
          << static_cast<uint8_t>(session.status);
    }
    return p;
}
// JoinRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, JoinRequest& e) {
    p >> e.session_id;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const JoinRequest& e) {
    p << e.session_id;
    return p;
}
// JoinResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, JoinResponse&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const JoinResponse&) { return p; }
// LeaveRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, LeaveRequest&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const LeaveRequest&) { return p; }
// LeaveResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, LeaveResponse&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const LeaveResponse&) {
    return p;
}
}  // namespace dto_session
