#include "common/dto/dto_game.h"

namespace dto_game {

// TurnRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, TurnRequest& e) {
    e.direction = static_cast<TurnDirection>(p.get<uint8_t>());
    p >> e.turn;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const TurnRequest& e) {
    p << static_cast<uint8_t>(e.direction) << e.turn;
    return p;
}

// AccelerateRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, AccelerateRequest& e) {
    p >> e.accelerate;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const AccelerateRequest& e) {
    p << e.accelerate;
    return p;
}

// UseBoostRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, UseBoostRequest& e) {
    p >> e.useBoost;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const UseBoostRequest& e) {
    p << e.useBoost;
    return p;
}

// ReverseRequest SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, ReverseRequest& e) {
    p >> e.reverse;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const ReverseRequest& e) {
    p << e.reverse;
    return p;
}

// GameSnapshot SERIALIZABLE
ProtocolReceiver& operator>>(ProtocolReceiver& p, GameSnapshot& r) {
    p >> r.raceTimeLeft;

    r.players.resize(p.get<size_t>());
    for (auto& player : r.players) {
        p >> player.id >> player.name;

        CarSnapshot& car = player.car;
        car.type = static_cast<CarSpriteType>(p.get<uint8_t>());
        p >> car.x >> car.y >> car.vx >> car.vy >> car.angle >> car.speed >>
            car.health >> car.nitroActive >> car.braking >> car.accelerating;

        RaceProgressSnapshot& progress = player.raceProgress;
        p >> progress.playerId >> progress.nextCheckpoint >>
            progress.finished >> progress.disqualified >> progress.elapsedTime;
    }
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const GameSnapshot& r) {
    p << r.raceTimeLeft;

    p << r.players.size();
    for (const auto& player : r.players) {
        p << player.id << player.name;

        const CarSnapshot& car = player.car;
        p << static_cast<uint8_t>(car.type);
        p << car.x << car.y << car.vx << car.vy << car.angle << car.speed
          << car.health << car.nitroActive << car.braking << car.accelerating;

        const RaceProgressSnapshot& progress = player.raceProgress;
        p << progress.playerId << progress.nextCheckpoint << progress.finished
          << progress.disqualified << progress.elapsedTime;
    }
    return p;
}

}  // namespace dto_game
