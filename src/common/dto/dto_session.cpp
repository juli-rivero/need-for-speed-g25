#include "common/dto/dto_session.h"

namespace dto_session {

// StartRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, StartRequest& e) {
    p >> e.ready;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const StartRequest& e) {
    p << e.ready;
    return p;
}

// StartResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, StartResponse&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const StartResponse&) {
    return p;
}

// LeaveRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, LeaveRequest&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const LeaveRequest&) { return p; }
// LeaveResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, LeaveResponse&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const LeaveResponse&) {
    return p;
}

// ChooseCarRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, ChooseCarRequest& r) {
    p >> r.car_name;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const ChooseCarRequest& r) {
    p << r.car_name;
    return p;
}

// SessionSnapshot SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SessionSnapshot& r) {
    SessionConfig& session = r.session;
    p >> session.city >> session.maxPlayers >> session.name >>
        session.raceCount;

    auto& players = r.players;
    players.resize(p.get<size_t>());
    for (auto& player : players) {
        player.carType = static_cast<CarSpriteType>(p.get<uint8_t>());
        p >> player.id >> player.name >> player.isHost >> player.isReady;
    }
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const SessionSnapshot& r) {
    const SessionConfig& session = r.session;
    p << session.city << session.maxPlayers << session.name
      << session.raceCount;

    const auto& players = r.players;
    p << players.size();
    for (const auto& player : players) {
        p << static_cast<uint8_t>(player.carType);
        p << player.id << player.name << player.isHost << player.isReady;
    }
    return p;
}

}  // namespace dto_session
