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
    return p >> r.map >> r.info;
}
ProtocolSender& operator<<(ProtocolSender& p, const StartResponse& r) {
    return p << r.map << r.info;
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
    return p >> r.car_name;
}
ProtocolSender& operator<<(ProtocolSender& p, const ChooseCarRequest& r) {
    return p << r.car_name;
}

// SessionSnapshot SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SessionSnapshot& r) {
    return p >> r.session >> r.players;
}
ProtocolSender& operator<<(ProtocolSender& p, const SessionSnapshot& r) {
    return p << r.session << r.players;
}
