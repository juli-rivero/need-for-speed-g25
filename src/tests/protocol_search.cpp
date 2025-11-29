#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "common/dto/dto_search.h"
#include "common/structs.h"
#include "tests/protocol_mock_class.h"

// SearchRequest: cliente -> servidor
TEST_F(ProtocolTest, SearchRequestRoundTrip) {
    client_protocol << dto_search::SearchRequest{} << ProtocolSender::send;

    server_protocol.get<dto_search::SearchRequest>();

    SUCCEED();  // Si deserializa sin errores, la prueba pasa
}

// SearchResponse: servidor -> cliente
TEST_F(ProtocolTest, SearchResponseEmpty) {
    server_protocol << dto_search::SearchResponse{.sessions = {}}
                    << ProtocolSender::send;

    const auto received = client_protocol.get<dto_search::SearchResponse>();
    EXPECT_TRUE(received.sessions.empty());
}

TEST_F(ProtocolTest, SearchResponseWithSessions) {
    const dto_search::SearchResponse resp{.sessions{
        {.name = "Room A",
         .maxPlayers = 4,
         .raceCount = 2,
         .city = "NeoCity",
         .currentPlayers = 1,
         .status = SessionStatus::Waiting},
        {.name = "Room B",
         .maxPlayers = 8,
         .raceCount = 3,
         .city = "OldTown",
         .currentPlayers = 5,
         .status = SessionStatus::Playing},
    }};

    server_protocol << resp << ProtocolSender::send;

    const auto received = client_protocol.get<dto_search::SearchResponse>();
    ASSERT_EQ(received.sessions.size(), resp.sessions.size());
    for (size_t i = 0; i < resp.sessions.size(); ++i) {
        const auto& a = resp.sessions[i];
        const auto& b = received.sessions[i];
        EXPECT_EQ(a.name, b.name);
        EXPECT_EQ(a.maxPlayers, b.maxPlayers);
        EXPECT_EQ(a.raceCount, b.raceCount);
        EXPECT_EQ(a.city, b.city);
        EXPECT_EQ(a.currentPlayers, b.currentPlayers);
        EXPECT_EQ(static_cast<int>(a.status), static_cast<int>(b.status));
    }
}

// JoinRequest: cliente -> servidor
TEST_F(ProtocolTest, JoinRequestRoundTrip) {
    const std::string session_id = "room-xyz";
    const dto_search::JoinRequest req{.session_id = session_id};

    client_protocol << req << ProtocolSender::send;

    const auto received = server_protocol.get<dto_search::JoinRequest>();
    EXPECT_EQ(received.session_id, session_id);
}

// JoinResponse: servidor -> cliente
TEST_F(ProtocolTest, JoinResponseRoundTrip) {
    const dto_search::JoinResponse resp{
        .session = {.name = "Main Room",
                    .maxPlayers = 6,
                    .raceCount = 4,
                    .city = "Kamchatka",
                    .currentPlayers = 2,
                    .status = SessionStatus::Waiting},
        .carTypes{
            {.type = CarType::Speedster,
             .display{.name = "Speedster", .description = "Fast and light"},
             .stats{.maxSpeed = 11.f,
                    .acceleration = 12.f,
                    .mass = 13.f,
                    .control = 14.f,
                    .maxHealth = 15.f,
                    .nitroMultiplier = 16.f,
                    .nitroDuration = 17.f,
                    .nitroCooldown = 18.f,
                    .width = 19.f,
                    .height = 20.f,
                    .density = 21.f,
                    .friction = 22.f,
                    .restitution = 23.f,
                    .linearDamping = 24.f,
                    .angularDamping = 25.f}},
            {.type = CarType::Tank,
             .display{.name = "Tank", .description = "Heavy and tough"},
             .stats{.maxSpeed = 21.f,
                    .acceleration = 22.f,
                    .mass = 23.f,
                    .control = 24.f,
                    .maxHealth = 25.f,
                    .nitroMultiplier = 26.f,
                    .nitroDuration = 27.f,
                    .nitroCooldown = 28.f,
                    .width = 29.f,
                    .height = 30.f,
                    .density = 31.f,
                    .friction = 32.f,
                    .restitution = 33.f,
                    .linearDamping = 34.f,
                    .angularDamping = 35.f}},
        },
    };

    server_protocol << resp << ProtocolSender::send;

    const auto received = client_protocol.get<dto_search::JoinResponse>();

    // SessionInfo
    EXPECT_EQ(received.session.name, resp.session.name);
    EXPECT_EQ(received.session.maxPlayers, resp.session.maxPlayers);
    EXPECT_EQ(received.session.raceCount, resp.session.raceCount);
    EXPECT_EQ(received.session.city, resp.session.city);
    EXPECT_EQ(received.session.currentPlayers, resp.session.currentPlayers);
    EXPECT_EQ(static_cast<int>(received.session.status),
              static_cast<int>(resp.session.status));

    // CarInfo
    ASSERT_EQ(received.carTypes.size(), resp.carTypes.size());
    for (size_t i = 0; i < resp.carTypes.size(); ++i) {
        const auto& a = resp.carTypes[i];
        const auto& b = received.carTypes[i];
        EXPECT_EQ(static_cast<int>(a.type), static_cast<int>(b.type));
        EXPECT_EQ(a.display.name, b.display.name);
        EXPECT_EQ(a.display.description, b.display.description);

        EXPECT_FLOAT_EQ(a.stats.maxSpeed, b.stats.maxSpeed);
        EXPECT_FLOAT_EQ(a.stats.acceleration, b.stats.acceleration);
        EXPECT_FLOAT_EQ(a.stats.mass, b.stats.mass);
        EXPECT_FLOAT_EQ(a.stats.control, b.stats.control);
        EXPECT_FLOAT_EQ(a.stats.maxHealth, b.stats.maxHealth);
        EXPECT_FLOAT_EQ(a.stats.nitroMultiplier, b.stats.nitroMultiplier);
        EXPECT_FLOAT_EQ(a.stats.nitroDuration, b.stats.nitroDuration);
        EXPECT_FLOAT_EQ(a.stats.nitroCooldown, b.stats.nitroCooldown);
        EXPECT_FLOAT_EQ(a.stats.width, b.stats.width);
        EXPECT_FLOAT_EQ(a.stats.height, b.stats.height);
        EXPECT_FLOAT_EQ(a.stats.density, b.stats.density);
        EXPECT_FLOAT_EQ(a.stats.friction, b.stats.friction);
        EXPECT_FLOAT_EQ(a.stats.restitution, b.stats.restitution);
        EXPECT_FLOAT_EQ(a.stats.linearDamping, b.stats.linearDamping);
        EXPECT_FLOAT_EQ(a.stats.angularDamping, b.stats.angularDamping);
    }
}

// CreateRequest: cliente -> servidor
TEST_F(ProtocolTest, CreateRequestRoundTrip) {
    const dto_search::CreateRequest req{
        .config = {.name = "Nueva Sala",
                   .maxPlayers = 5,
                   .raceCount = 2,
                   .city = "LibertyCity"},
    };

    client_protocol << req << ProtocolSender::send;

    const auto received = server_protocol.get<dto_search::CreateRequest>();
    EXPECT_EQ(received.config.name, req.config.name);
    EXPECT_EQ(received.config.maxPlayers, req.config.maxPlayers);
    EXPECT_EQ(received.config.raceCount, req.config.raceCount);
    EXPECT_EQ(received.config.city, req.config.city);
}
