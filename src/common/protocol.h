#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "common/buffer.h"
#include "common/socket.h"

class ProtocolReceiver;

struct ClosedProtocol final : std::runtime_error {
    explicit ClosedProtocol(const std::string& e = "")
        : std::runtime_error("The protocol is closed: " + e) {}
};

class ProtocolReceiver {
   public:
    virtual ~ProtocolReceiver() = default;

    virtual bool is_stream_recv_closed() = 0;
    virtual void close_stream_recv() = 0;

    template <typename T>
    T get() {
        T t;
        *this >> t;
        return t;
    }

#define DECLARE_VIRTUAL_RECV(Type) \
    virtual ProtocolReceiver& operator>>(Type& e) = 0;

    DECLARE_VIRTUAL_RECV(bool)
    DECLARE_VIRTUAL_RECV(uint8_t)
    DECLARE_VIRTUAL_RECV(uint16_t)
    DECLARE_VIRTUAL_RECV(uint32_t)
    DECLARE_VIRTUAL_RECV(size_t)
    DECLARE_VIRTUAL_RECV(float)
    DECLARE_VIRTUAL_RECV(std::string)

#undef DECLARE_VIRTUAL_RECV

    template <typename T>
    ProtocolReceiver& operator>>(std::vector<T>& v) {
        v.resize(get<size_t>());
        for (auto& elem : v) *this >> elem;
        return *this;
    }
};

class ProtocolSender {
   public:
    virtual ~ProtocolSender() = default;

    virtual bool is_stream_send_closed() = 0;
    virtual void close_stream_send() = 0;

    virtual void send() = 0;

    static ProtocolSender& send(ProtocolSender& protocol);

    /// Util for accepting functions with insertion operator. @see
    /// protocol::send
    ProtocolSender& operator<<(ProtocolSender& (*const func)(ProtocolSender&)) {
        return func(*this);
    }

#define DECLARE_VIRTUAL_SEND(Type) \
    virtual ProtocolSender& operator<<(const Type& e) = 0;

    DECLARE_VIRTUAL_SEND(bool)
    DECLARE_VIRTUAL_SEND(uint8_t)
    DECLARE_VIRTUAL_SEND(uint16_t)
    DECLARE_VIRTUAL_SEND(uint32_t)
    DECLARE_VIRTUAL_SEND(size_t)
    DECLARE_VIRTUAL_SEND(float)
    DECLARE_VIRTUAL_SEND(std::string)

#undef DECLARE_VIRTUAL_SEND

    template <typename T>
    ProtocolSender& operator<<(const std::vector<T>& v) {
        *this << static_cast<size_t>(v.size());
        for (const auto& elem : v) *this << static_cast<T>(elem);
        return *this;
    }
};

/** Protocol responsable for encoding and decoding data communicating with
 * itself using a socket.
 *
 * Reduces overload of packets as it buffers all data and, when it is ready,
 * sends all data as a block. This helps as it tries to maximize the payload of
 * the packet.
 *
 * This implementation offers primitive buffering using `htons`, `htonl`,
 * `ntohs` and `ntohl` when appropriate.
 *
 * This class is intended to be extended implementing the insertion/extraction
 * operator as a friend function in other classes.
 */
class Protocol final : public ProtocolSender, public ProtocolReceiver {
    Socket socket;
    Buffer buffer;

   public:
    explicit Protocol(Socket&& socket);

    template <typename... Args>
    explicit Protocol(Args&&... args) : socket(std::forward<Args>(args)...) {}

    Protocol(const Protocol&) = delete;
    Protocol& operator=(const Protocol&) = delete;

    Protocol(Protocol&&) noexcept = default;
    Protocol& operator=(Protocol&&) noexcept = default;

    using ProtocolSender::operator<<;
    using ProtocolReceiver::operator>>;

    [[nodiscard]] Protocol accept() const;

    bool is_stream_recv_closed() override;
    bool is_stream_send_closed() override;
    void close_stream_recv() override;
    void close_stream_send() override;
    void close();

    void send() override;

#define DECLARE_OVERRIDE_SERIALIZABLE(Type)       \
    ProtocolReceiver& operator>>(Type&) override; \
    ProtocolSender& operator<<(const Type&) override;

    DECLARE_OVERRIDE_SERIALIZABLE(bool)
    DECLARE_OVERRIDE_SERIALIZABLE(uint8_t)
    DECLARE_OVERRIDE_SERIALIZABLE(uint16_t)
    DECLARE_OVERRIDE_SERIALIZABLE(uint32_t)
    DECLARE_OVERRIDE_SERIALIZABLE(size_t)
    DECLARE_OVERRIDE_SERIALIZABLE(float)
    DECLARE_OVERRIDE_SERIALIZABLE(std::string)

#undef DECLARE_OVERRIDE_SERIALIZABLE

   private:
    void recv_from_socket(void* const& value, size_t sz);
};
