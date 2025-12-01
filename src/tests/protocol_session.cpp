#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "common/dto/dto_session.h"
#include "common/structs.h"
#include "tests/protocol_mock_class.h"

// StartRequest: cliente -> servidor
TEST_F(ProtocolTest, SessionStartRequestRoundTrip) {
    const dto_session::StartRequest req_true{.ready = true};
    const dto_session::StartRequest req_false{.ready = false};

    client_protocol << req_true << req_false << ProtocolSender::send;

    dto_session::StartRequest received_true, received_false;
    server_protocol >> received_true >> received_false;

    EXPECT_TRUE(req_true.ready);
    EXPECT_FALSE(req_false.ready);
}

// StartResponse: servidor -> cliente
TEST_F(ProtocolTest, SessionStartResponseRoundTrip) {
    const dto_session::StartResponse resp{
        .city_info = {.name = "Buenos Aires",
                      .walls = {{.pos{.x = 0.f, .y = 0.f},
                                 .width = 10.f,
                                 .height = 1.f}},
                      .bridges = {{.pos{.x = 5.f, .y = 5.f},
                                   .width = 3.f,
                                   .height = 1.f}},
                      .railings = {},
                      .overpasses = {}},
        .first_race_info =
            {.name = "Opening Race",
             .checkpoints = {{.order = 0,
                              .bound = {.pos{.x = 1.f, .y = 2.f},
                                        .width = 2.f,
                                        .height = 1.f},
                              .angle = 0.5f,
                              .type = CheckpointType::Start},
                             {.order = 1,
                              .bound = {.pos{.x = 3.f, .y = 4.f},
                                        .width = 2.f,
                                        .height = 1.f},
                              .angle = 1.0f,
                              .type = CheckpointType::Finish}},
             .spawnPoints = {{.pos{.x = 1.f, .y = 1.f}, .angle = 0.f},
                             {.pos{.x = 2.f, .y = 1.f}, .angle = 0.f}}},
        .upgrade_choices = {{.stat = UpgradeStat::Acceleration,
                             .delta = 1.0f,
                             .penalty = 1.0f}},
    };

    server_protocol << resp << ProtocolSender::send;

    const auto received = client_protocol.get<dto_session::StartResponse>();

    EXPECT_EQ(received.city_info.name, resp.city_info.name);
    ASSERT_EQ(received.city_info.walls.size(), resp.city_info.walls.size());
    if (!resp.city_info.walls.empty()) {
        EXPECT_FLOAT_EQ(received.city_info.walls[0].pos.x,
                        resp.city_info.walls[0].pos.x);
        EXPECT_FLOAT_EQ(received.city_info.walls[0].pos.y,
                        resp.city_info.walls[0].pos.y);
    }

    EXPECT_EQ(received.first_race_info.name, resp.first_race_info.name);
    ASSERT_EQ(received.first_race_info.checkpoints.size(),
              resp.first_race_info.checkpoints.size());
    if (!resp.first_race_info.checkpoints.empty()) {
        EXPECT_EQ(received.first_race_info.checkpoints[0].order,
                  resp.first_race_info.checkpoints[0].order);
        EXPECT_EQ(
            static_cast<int>(received.first_race_info.checkpoints[0].type),
            static_cast<int>(resp.first_race_info.checkpoints[0].type));
    }
}

// LeaveRequest: cliente -> servidor
TEST_F(ProtocolTest, SessionLeaveRequestRoundTrip) {
    client_protocol << dto_session::LeaveRequest{} << ProtocolSender::send;

    server_protocol.get<dto_session::LeaveRequest>();
    SUCCEED();
}

// LeaveResponse: servidor -> cliente
TEST_F(ProtocolTest, SessionLeaveResponseRoundTrip) {
    server_protocol << dto_session::LeaveResponse{} << ProtocolSender::send;

    client_protocol.get<dto_session::LeaveResponse>();
    SUCCEED();
}

// ChooseCarRequest: cliente -> servidor
TEST_F(ProtocolTest, SessionChooseCarRequestRoundTrip) {
    const dto_session::ChooseCarRequest req{.car_type = CarType::Tank};

    client_protocol << req << ProtocolSender::send;

    const auto received = server_protocol.get<dto_session::ChooseCarRequest>();
    EXPECT_EQ(static_cast<int>(received.car_type),
              static_cast<int>(req.car_type));
}

// SessionSnapshot: servidor -> cliente
TEST_F(ProtocolTest, SessionSnapshotRoundTrip) {
    const dto_session::SessionSnapshot snap{
        .session = {.name = "Sala A",
                    .maxPlayers = 6,
                    .raceCount = 3,
                    .city = "Rosario"},
        .players = {{.id = 1u,
                     .name = "Pablito",
                     .carType = CarType::Speedster,
                     .isReady = true,
                     .isHost = true},
                    {.id = 2u,
                     .name = "Bob",
                     .carType = CarType::Tank,
                     .isReady = false,
                     .isHost = false}},
    };

    server_protocol << snap << ProtocolSender::send;

    const auto received = client_protocol.get<dto_session::SessionSnapshot>();
    EXPECT_EQ(received.session.name, snap.session.name);
    EXPECT_EQ(received.session.maxPlayers, snap.session.maxPlayers);
    EXPECT_EQ(received.session.raceCount, snap.session.raceCount);
    EXPECT_EQ(received.session.city, snap.session.city);

    ASSERT_EQ(received.players.size(), snap.players.size());
    for (size_t i = 0; i < snap.players.size(); ++i) {
        const auto& a = snap.players[i];
        const auto& b = received.players[i];
        EXPECT_EQ(a.id, b.id);
        EXPECT_EQ(a.name, b.name);
        EXPECT_EQ(static_cast<int>(a.carType), static_cast<int>(b.carType));
        EXPECT_EQ(a.isReady, b.isReady);
        EXPECT_EQ(a.isHost, b.isHost);
    }
}
