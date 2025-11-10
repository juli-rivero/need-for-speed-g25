#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../common/protocol.h"
#include "../common/socket.h"

class ProtocolTest : public ::testing::Test {
   protected:
    Socket acceptor;
    Protocol client_protocol;
    Protocol server_protocol;

    ProtocolTest()
        : acceptor("7649"),
          client_protocol("localhost", "7649"),
          server_protocol(acceptor.accept()) {}
};

TEST_F(ProtocolTest, Bool) {
    constexpr bool original = true;

    client_protocol << original << ProtocolSender::send;

    const bool received = server_protocol.get<bool>();

    EXPECT_EQ(original, received);
}

TEST_F(ProtocolTest, Uint8) {
    constexpr uint8_t original = 42;

    client_protocol << original << ProtocolSender::send;

    const auto received = server_protocol.get<uint8_t>();

    EXPECT_EQ(original, received);
}

TEST_F(ProtocolTest, Uint16) {
    constexpr uint16_t original = 65535;

    client_protocol << original << ProtocolSender::send;

    const auto received = server_protocol.get<uint16_t>();

    EXPECT_EQ(original, received);
}

TEST_F(ProtocolTest, Uint32) {
    constexpr uint32_t original = 123456789;

    client_protocol << original << ProtocolSender::send;

    const auto received = server_protocol.get<uint32_t>();

    EXPECT_EQ(original, received);
}

TEST_F(ProtocolTest, Float) {
    constexpr float original = 3.14f;

    client_protocol << original << ProtocolSender::send;

    const auto received = server_protocol.get<float>();

    EXPECT_FLOAT_EQ(original, received);
}

TEST_F(ProtocolTest, String) {
    const std::string original = "Hello, world!";

    client_protocol << original << ProtocolSender::send;

    const auto received = server_protocol.get<std::string>();

    EXPECT_EQ(original, received);
}

TEST_F(ProtocolTest, Vector) {
    const std::vector<uint8_t> original = {1, 2, 3, 4, 5};

    client_protocol << original << ProtocolSender::send;

    const auto received = server_protocol.get<std::vector<uint8_t>>();

    EXPECT_EQ(original, received);
}

TEST_F(ProtocolTest, MultipleTypesSingleSend) {
    constexpr bool original_bool = true;
    constexpr uint8_t original_uint8 = 42;
    constexpr uint16_t original_uint16 = 65535;
    constexpr uint32_t original_uint32 = 123456789;
    constexpr float original_float = 3.14f;
    const std::string original_string = "Hello, Protocol!";
    const std::vector<uint8_t> original_vector = {10, 20, 30, 40, 50};

    client_protocol << original_bool << original_uint8 << original_uint16
                    << original_uint32 << original_float << original_string
                    << original_vector << ProtocolSender::send;

    bool received_bool = false;
    uint8_t received_uint8 = 0;
    uint16_t received_uint16 = 0;
    uint32_t received_uint32 = 0;
    float received_float = 0.0f;
    std::string received_string;
    std::vector<uint8_t> received_vector;

    server_protocol >> received_bool >> received_uint8 >> received_uint16 >>
        received_uint32 >> received_float >> received_string >> received_vector;

    EXPECT_EQ(original_bool, received_bool);
    EXPECT_EQ(original_uint8, received_uint8);
    EXPECT_EQ(original_uint16, received_uint16);
    EXPECT_EQ(original_uint32, received_uint32);
    EXPECT_FLOAT_EQ(original_float, received_float);
    EXPECT_EQ(original_string, received_string);
    EXPECT_EQ(original_vector, received_vector);
}
