#include "common/dto/dto_session.h"

#include "common/dto/structs_serializable.h"

using namespace dto_session;

// StartRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, StartRequest& e) {
    return p >> e.ready;
}
ProtocolSender& operator<<(ProtocolSender& p, const StartRequest& e) {
    return p << e.ready;
}

// StartResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, StartResponse& r) {
    return p >> r.city_info >> r.first_race_info;
}
ProtocolSender& operator<<(ProtocolSender& p, const StartResponse& r) {
    return p << r.city_info << r.first_race_info;
}

// LeaveRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, LeaveRequest&) {
    p.get<uint8_t>();
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const LeaveRequest&) {
    return p << static_cast<uint8_t>(0);
}
// LeaveResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, LeaveResponse&) {
    p.get<uint8_t>();
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const LeaveResponse&) {
    return p << static_cast<uint8_t>(0);
}

// ChooseCarRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, ChooseCarRequest& r) {
    return p >> r.car_type;
}
ProtocolSender& operator<<(ProtocolSender& p, const ChooseCarRequest& r) {
    return p << r.car_type;
}

// SessionSnapshot SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SessionSnapshot& r) {
    return p >> r.session >> r.players;
}
ProtocolSender& operator<<(ProtocolSender& p, const SessionSnapshot& r) {
    return p << r.session << r.players;
}
