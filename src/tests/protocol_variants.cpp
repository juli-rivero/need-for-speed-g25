// Tests de variantes del protocolo: verificar que al enviar un std::variant
// (dto::Request / dto::Response) con una clase X, se recibe como la misma X.

#include <gtest/gtest.h>

#include <string>

#include "common/dto/dto.h"
#include "common/structs.h"
#include "tests/protocol_mock_class.h"

using std::holds_alternative;

// Requests: cliente -> servidor

TEST_F(ProtocolTest, RequestVariantJoinRequest) {
    const std::string session_id = "room-123";
    dto::Request req = dto_search::JoinRequest{session_id};

    client_protocol << req << ProtocolSender::send;

    const auto received = server_protocol.get<dto::Request>();

    ASSERT_TRUE(holds_alternative<dto_search::JoinRequest>(received));
    EXPECT_FALSE(holds_alternative<dto_game::TurnRequest>(received));
    EXPECT_FALSE(holds_alternative<dto_search::SearchRequest>(received));
    EXPECT_FALSE(holds_alternative<dto_session::LeaveRequest>(received));
    const auto& jr = std::get<dto_search::JoinRequest>(received);
    EXPECT_EQ(jr.session_id, session_id);
}

TEST_F(ProtocolTest, RequestVariantTurnRequest) {
    const TurnDirection dir = TurnDirection::Left;
    const bool turn = true;
    dto::Request req = dto_game::TurnRequest{dir, turn};

    client_protocol << req << ProtocolSender::send;

    const auto received = server_protocol.get<dto::Request>();

    ASSERT_TRUE(holds_alternative<dto_game::TurnRequest>(received));
    EXPECT_FALSE(holds_alternative<dto_game::AccelerateRequest>(received));
    EXPECT_FALSE(holds_alternative<dto_session::StartRequest>(received));
    const auto& tr = std::get<dto_game::TurnRequest>(received);
    EXPECT_EQ(static_cast<int>(tr.direction), static_cast<int>(dir));
    EXPECT_EQ(tr.turn, turn);
}

// Responses: servidor -> cliente

TEST_F(ProtocolTest, ResponseVariantErrorResponse) {
    const std::string message = "algo salio mal";
    dto::Response resp = dto::ErrorResponse{message};

    server_protocol << resp << ProtocolSender::send;

    const auto received = client_protocol.get<dto::Response>();

    ASSERT_TRUE(holds_alternative<dto::ErrorResponse>(received));
    EXPECT_FALSE(holds_alternative<dto_session::LeaveResponse>(received));
    EXPECT_FALSE(holds_alternative<dto_search::SearchResponse>(received));
    const auto& er = std::get<dto::ErrorResponse>(received);
    EXPECT_EQ(er.message, message);
}

TEST_F(ProtocolTest, ResponseVariantLeaveResponse) {
    dto::Response resp = dto_session::LeaveResponse{};

    server_protocol << resp << ProtocolSender::send;

    const auto received = client_protocol.get<dto::Response>();

    ASSERT_TRUE(holds_alternative<dto_session::LeaveResponse>(received));
    EXPECT_FALSE(holds_alternative<dto::ErrorResponse>(received));
    EXPECT_FALSE(holds_alternative<dto_search::SearchResponse>(received));
}
