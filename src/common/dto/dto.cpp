#include "common/dto/dto.h"

namespace dto {

// ErrorResponse SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, dto::ErrorResponse& e) {
    p >> e.message;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const dto::ErrorResponse& e) {
    p << e.message;
    return p;
}

// Request SENDABLE
ProtocolSender& operator<<(ProtocolSender& p, const Request& e) {
    p << static_cast<uint8_t>(e.type);
    std::visit([&](const auto& val) { p << val; }, e.body);
    return p;
}

// Response SENDABLE
ProtocolSender& operator<<(ProtocolSender& p, const Response& e) {
    p << static_cast<uint8_t>(e.type);
    std::visit([&](const auto& val) { p << val; }, e.body);
    return p;
}

}  // namespace dto
