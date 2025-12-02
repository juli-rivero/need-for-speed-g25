#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "common/dto/dto_game.h"
#include "common/structs.h"
#include "tests/protocol_mock_class.h"

// GameStaticSnapshot: servidor -> cliente
TEST_F(ProtocolTest, GameStaticSnapshotRoundTrip) {
    const dto_game::GameStaticSnapshot snap{
        .new_race_info = {.name = "Race 2",
                          .checkpoints = {{.order = 0,
                                           .bound = {.pos{.x = 10.f, .y = 10.f},
                                                     .width = 3.f,
                                                     .height = 1.f},
                                           .angle = 0.25f,
                                           .type = CheckpointType::Start}},
                          .spawnPoints = {{.pos{.x = 0.f, .y = 0.f},
                                           .angle = 0.f}}},
    };

    server_protocol << snap << ProtocolSender::send;

    const auto received = client_protocol.get<dto_game::GameStaticSnapshot>();
    EXPECT_EQ(received.new_race_info.name, snap.new_race_info.name);
    ASSERT_EQ(received.new_race_info.checkpoints.size(),
              snap.new_race_info.checkpoints.size());
}

// TurnRequest: cliente -> servidor
TEST_F(ProtocolTest, GameTurnRequestRoundTrip) {
    const dto_game::TurnRequest req{.direction = TurnDirection::Left,
                                    .turn = true};

    client_protocol << req << ProtocolSender::send;

    const auto received = server_protocol.get<dto_game::TurnRequest>();
    EXPECT_EQ(static_cast<int>(received.direction),
              static_cast<int>(req.direction));
    EXPECT_EQ(received.turn, req.turn);
}

// AccelerateRequest: cliente -> servidor
TEST_F(ProtocolTest, GameAccelerateRequestRoundTrip) {
    const dto_game::AccelerateRequest req{.accelerate = true};
    client_protocol << req << ProtocolSender::send;
    const auto received = server_protocol.get<dto_game::AccelerateRequest>();
    EXPECT_EQ(received.accelerate, req.accelerate);
}

// UseBoostRequest: cliente -> servidor
TEST_F(ProtocolTest, GameUseBoostRequestRoundTrip) {
    const dto_game::UseBoostRequest req{.useBoost = true};
    client_protocol << req << ProtocolSender::send;
    const auto received = server_protocol.get<dto_game::UseBoostRequest>();
    EXPECT_EQ(received.useBoost, req.useBoost);
}

// ReverseRequest: cliente -> servidor
TEST_F(ProtocolTest, GameReverseRequestRoundTrip) {
    const dto_game::ReverseRequest req{.reverse = true};
    client_protocol << req << ProtocolSender::send;
    const auto received = server_protocol.get<dto_game::ReverseRequest>();
    EXPECT_EQ(received.reverse, req.reverse);
}

// GameSnapshotPacket: servidor -> cliente
TEST_F(ProtocolTest, GameSnapshotPacketRoundTrip) {
    const GameSnapshot snapshot{
        .players = {{
            .id = 7u,
            .name = std::string("Alice"),
            .car = {.type = CarType::Speedster,
                    .layer = RenderLayer::OVER,
                    .bound = {.pos{.x = 1.f, .y = 2.f},
                              .width = 1.5f,
                              .height = 0.9f},
                    .vx = 3.f,
                    .vy = 4.f,
                    .angle = 0.1f,
                    .speed = 5.f,
                    .health = 100.f,
                    .nitroActive = false,
                    .braking = false,
                    .accelerating = true},
            .raceProgress = {.nextCheckpoint = 2u,
                             .finished = false,
                             .disqualified = false,
                             .elapsed = 9.5f,
                             .penaltyTime = 0.0f},
            .upgrades = {.bonusMaxSpeed = 2.0f,
                         .bonusAcceleration = 0.5f,
                         .bonusHealth = 1.0f,
                         .bonusNitro = 3.0f},
            .rawTime = 2.0f,
            .penalty = 1.0f,
            .netTime = 3.0f,
        }},
        .npcs =
            {{.type = CarType::Tank,
              .layer = RenderLayer::UNDER,
              .bound = {.pos{.x = 5.f, .y = 6.f}, .width = 2.f, .height = 1.f},
              .angle = 0.2f}},
        .match = {.matchState = MatchState::Racing,
                  .currentRaceIndex = 1u,
                  .time = 42.0f,
                  .positions = {7}},
        .race = {.raceState = RaceState::Running,
                 .raceElapsed = 10.0f,
                 .raceCountdown = 0.0f,
                 .raceTimeLeft = 120.0f,
                 .positions = {7}},
    };

    const dto_game::GameSnapshotPacket packet{.snapshot = snapshot};

    server_protocol << packet << ProtocolSender::send;

    const auto received = client_protocol.get<dto_game::GameSnapshotPacket>();

    EXPECT_EQ(static_cast<int>(received.snapshot.match.matchState),
              static_cast<int>(snapshot.match.matchState));
    EXPECT_FLOAT_EQ(received.snapshot.race.raceElapsed,
                    snapshot.race.raceElapsed);
    ASSERT_EQ(received.snapshot.players.size(), snapshot.players.size());
    if (!snapshot.players.empty()) {
        EXPECT_EQ(received.snapshot.players[0].id, snapshot.players[0].id);
        EXPECT_EQ(received.snapshot.players[0].name, snapshot.players[0].name);
        EXPECT_EQ(static_cast<int>(received.snapshot.players[0].car.type),
                  static_cast<int>(snapshot.players[0].car.type));
    }
}

// EventPacket: servidor -> cliente (CollisionSimple)
TEST_F(ProtocolTest, GameEventPacketCollisionSimpleRoundTrip) {
    const CollisionEvent ev = CollisionSimple{.player = 3u, .intensity = 1.5f};
    const dto_game::EventPacket pkt{.collision = ev};

    server_protocol << pkt << ProtocolSender::send;

    const auto received = client_protocol.get<dto_game::EventPacket>();
    ASSERT_TRUE(std::holds_alternative<CollisionSimple>(received.collision));
    ASSERT_FALSE(std::holds_alternative<CollisionCarToCar>(received.collision));
    const auto& c = std::get<CollisionSimple>(received.collision);
    EXPECT_EQ(c.player, 3u);
    EXPECT_FLOAT_EQ(c.intensity, 1.5f);
}

// EventPacket: servidor -> cliente (CollisionCarToCar)
TEST_F(ProtocolTest, GameEventPacketCarToCarRoundTrip) {
    const CollisionEvent ev = CollisionCarToCar{5u, 8u, 3.25f};
    const dto_game::EventPacket pkt{.collision = ev};

    server_protocol << pkt << ProtocolSender::send;

    const auto received = client_protocol.get<dto_game::EventPacket>();
    ASSERT_TRUE(std::holds_alternative<CollisionCarToCar>(received.collision));
    ASSERT_FALSE(std::holds_alternative<CollisionSimple>(received.collision));
    const auto& c = std::get<CollisionCarToCar>(received.collision);
    EXPECT_EQ(c.player, 5u);
    EXPECT_EQ(c.other, 8u);
    EXPECT_FLOAT_EQ(c.intensity, 3.25f);
}
