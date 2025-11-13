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

}  // namespace dto_session
