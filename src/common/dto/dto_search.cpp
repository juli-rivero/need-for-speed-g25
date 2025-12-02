#include "common/dto/dto_search.h"

#include "common/dto/structs_serializable.h"

using namespace dto_search;

// SearchRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SearchRequest&) {
    p.get<uint8_t>();  // para que al enviar, no se envien 0 bytes y salte un
                       // error
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const SearchRequest&) {
    return p << static_cast<uint8_t>(0);
}
// SearchResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SearchResponse& e) {
    return p >> e.sessions;
}
ProtocolSender& operator<<(ProtocolSender& p, const SearchResponse& e) {
    return p << e.sessions;
}
// JoinRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, JoinRequest& e) {
    return p >> e.session_id;
}
ProtocolSender& operator<<(ProtocolSender& p, const JoinRequest& e) {
    return p << e.session_id;
}
// JoinResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, JoinResponse& r) {
    return p >> r.session >> r.carTypes;
}
ProtocolSender& operator<<(ProtocolSender& p, const JoinResponse& r) {
    return p << r.session << r.carTypes;
}

// CreateRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, CreateRequest& e) {
    return p >> e.config;
}
ProtocolSender& operator<<(ProtocolSender& p, const CreateRequest& e) {
    return p << e.config;
}

// StaticSessionDataRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, StaticSessionDataRequest&) {
    p.get<uint8_t>();  // para que al enviar, no se envien 0 bytes y salte un
                       // error
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const StaticSessionDataRequest&) {
    return p << static_cast<uint8_t>(0);
}
// StaticSessionDataResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p,
                             StaticSessionDataResponse& r) {
    return p >> r.data;
}
ProtocolSender& operator<<(ProtocolSender& p,
                           const StaticSessionDataResponse& r) {
    return p << r.data;
}
