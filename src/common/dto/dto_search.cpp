#include "common/dto/dto_search.h"

namespace dto_search {

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
ProtocolReceiver& operator>>(ProtocolReceiver& p, JoinResponse& r) {
    SessionInfo& session = r.session;
    p >> session.city >> session.maxPlayers >> session.name >>
        session.raceCount >> session.currentPlayers;
    session.status = static_cast<SessionStatus>(p.get<uint8_t>());
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const JoinResponse& r) {
    const SessionInfo& session = r.session;
    p << session.city << session.maxPlayers << session.name << session.raceCount
      << session.currentPlayers << static_cast<uint8_t>(session.status);
    return p;
}

// CreateRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, CreateRequest& e) {
    p >> e.config.city >> e.config.maxPlayers >> e.config.name >>
        e.config.raceCount;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const CreateRequest& e) {
    p << e.config.city << e.config.maxPlayers << e.config.name
      << e.config.raceCount;
    return p;
}

// CreateResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, CreateResponse&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const CreateResponse&) {
    return p;
}

}  // namespace dto_search
