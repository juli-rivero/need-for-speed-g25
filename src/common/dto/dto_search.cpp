#include "common/dto/dto_search.h"

#include "common/dto/structs_serializable.h"

using namespace dto_search;

// SearchRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, SearchRequest&) { return p; }
ProtocolSender& operator<<(ProtocolSender& p, const SearchRequest&) {
    return p;
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
