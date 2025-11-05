#include "common/dto/dto_lobby.h"

namespace dto_lobby {

// StartRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_lobby::StartRequest& e) {
    p >> e.ready;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const dto_lobby::StartRequest& e) {
    p << e.ready;
    return p;
}

// StartResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto_lobby::StartResponse&) {
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const dto_lobby::StartResponse&) {
    return p;
}
}  // namespace dto_lobby
