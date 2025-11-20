#include "common/protocol.h"

#include <netinet/in.h>

#include <string>
#include <utility>
#include <vector>

#include "common/liberror.h"

ProtocolSender& ProtocolSender::send(ProtocolSender& protocol) {
    protocol.send();
    return protocol;
}

Protocol::Protocol(Socket&& socket) : socket(std::move(socket)) {}

Protocol Protocol::accept() const { return Protocol(socket.accept()); }

bool Protocol::is_stream_recv_closed() {
    return socket.is_stream_recv_closed();
}
bool Protocol::is_stream_send_closed() {
    return socket.is_stream_send_closed();
}
void Protocol::close_stream_recv() { socket.shutdown(SHUT_RD); }
void Protocol::close_stream_send() { socket.shutdown(SHUT_WR); }
void Protocol::close() { socket.close(); }

void Protocol::send() {
    try {
        if (socket.sendall(buffer.data(), buffer.size()) == 0) {
            socket.close();
            throw ClosedProtocol();
        }
    } catch (LibError& e) {
        socket.close();
        throw ClosedProtocol(e.what());
    }
    buffer.clear();
}

void Protocol::recv_from_socket(void* const& value, const size_t sz) {
    try {
        if (socket.recvall(value, sz) == 0) {
            socket.close();
            throw ClosedProtocol();
        }
    } catch (LibError& e) {
        socket.close();
        throw ClosedProtocol(e.what());
    }
}

ProtocolReceiver& Protocol::operator>>(bool& value) {
    value = get<uint8_t>();
    return *this;
}

ProtocolSender& Protocol::operator<<(const bool& value) {
    *this << static_cast<uint8_t>(value);
    return *this;
}

ProtocolReceiver& Protocol::operator>>(uint8_t& value) {
    recv_from_socket(&value, sizeof(value));
    return *this;
}

ProtocolSender& Protocol::operator<<(const uint8_t& value) {
    buffer.write<uint8_t>(value);
    return *this;
}

ProtocolReceiver& Protocol::operator>>(uint16_t& value) {
    recv_from_socket(&value, sizeof(value));
    value = ntohs(value);
    return *this;
}

ProtocolSender& Protocol::operator<<(const uint16_t& value) {
    const uint16_t to_send = htons(value);
    buffer.write<uint16_t>(to_send);
    return *this;
}

ProtocolReceiver& Protocol::operator>>(uint32_t& value) {
    recv_from_socket(&value, sizeof(value));
    value = ntohl(value);
    return *this;
}

ProtocolSender& Protocol::operator<<(const uint32_t& value) {
    buffer.write<uint32_t>(htonl(value));
    return *this;
}

ProtocolReceiver& Protocol::operator>>(size_t& value) {
    value = static_cast<size_t>(get<uint32_t>());
    return *this;
}

ProtocolSender& Protocol::operator<<(const size_t& value) {
    *this << static_cast<uint32_t>(value);
    return *this;
}

ProtocolReceiver& Protocol::operator>>(float& value) {
    value = static_cast<float>(static_cast<int32_t>(this->get<uint32_t>()));
    value /= 100;
    return *this;
}

ProtocolSender& Protocol::operator<<(const float& value) {
    *this << static_cast<uint32_t>(value * 100);
    return *this;
}

ProtocolReceiver& Protocol::operator>>(std::string& str) {
    const auto sz = get<uint16_t>();
    str.resize(sz);
    recv_from_socket(str.data(), sz);
    return *this;
}

ProtocolSender& Protocol::operator<<(const std::string& str) {
    *this << static_cast<uint16_t>(str.size());
    buffer.write_bytes(str.begin(), str.end());
    return *this;
}
