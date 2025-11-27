#include "common/dto/structs_serializable.h"

#include <variant>

#include "common/protocol.h"
#include "common/structs.h"

ProtocolSender& operator<<(ProtocolSender& p, const SessionConfig& s) {
    p << s.name << s.maxPlayers << s.raceCount << s.city;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, SessionConfig& s) {
    p >> s.name >> s.maxPlayers >> s.raceCount >> s.city;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const SessionInfo& s) {
    p << s.name << s.maxPlayers << s.raceCount << s.city;
    p << s.currentPlayers;
    p << s.status;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, SessionInfo& s) {
    p >> s.name >> s.maxPlayers >> s.raceCount >> s.city;
    p >> s.currentPlayers >> s.status;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const PlayerInfo& s) {
    p << s.id;
    p << s.name;
    p << s.carType;
    p << s.isReady;
    p << s.isHost;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, PlayerInfo& s) {
    p >> s.id;
    p >> s.name;
    p >> s.carType;
    p >> s.isReady;
    p >> s.isHost;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CarStaticInfo& s) {
    p << s.type;
    p << s.name;
    p << s.description;
    p << s.maxSpeed;
    p << s.acceleration;
    p << s.mass;
    p << s.control;
    p << s.health;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CarStaticInfo& s) {
    p >> s.type;
    p >> s.name;
    p >> s.description;
    p >> s.maxSpeed;
    p >> s.acceleration;
    p >> s.mass;
    p >> s.control;
    p >> s.health;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CollisionSimple& s) {
    p << s.player;
    p << s.intensity;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CollisionSimple& s) {
    p >> s.player;
    p >> s.intensity;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CollisionCarToCar& s) {
    // Serialize base part then derived field
    p << s.player;
    p << s.intensity;
    p << s.other;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CollisionCarToCar& s) {
    p >> s.player;
    p >> s.intensity;
    p >> s.other;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CollisionEvent& e) {
    // tag: 0 = CollisionSimple, 1 = CollisionCarToCar
    if (std::holds_alternative<CollisionCarToCar>(e)) {
        p << static_cast<uint8_t>(1);
        p << std::get<CollisionCarToCar>(e);
    } else {
        p << static_cast<uint8_t>(0);
        p << std::get<CollisionSimple>(e);
    }
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CollisionEvent& e) {
    uint8_t tag;
    p >> tag;
    if (tag == 1) {
        CollisionCarToCar cc(0, 0, 0.0f);
        p >> cc;
        e = cc;
    } else {
        CollisionSimple cs{0, 0.0f};
        p >> cs;
        e = cs;
    }
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CarSnapshot& s) {
    p << s.type;
    p << s.x << s.y;
    p << s.vx << s.vy;
    p << s.angle;
    p << s.speed;
    p << s.health;
    p << s.nitroActive;
    p << s.braking;
    p << s.accelerating;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CarSnapshot& s) {
    p >> s.type;
    p >> s.x >> s.y;
    p >> s.vx >> s.vy;
    p >> s.angle;
    p >> s.speed;
    p >> s.health;
    p >> s.nitroActive;
    p >> s.braking;
    p >> s.accelerating;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const RaceProgressSnapshot& s) {
    p << s.playerId;
    p << s.nextCheckpoint;
    p << s.finished;
    p << s.disqualified;
    p << s.elapsedTime;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, RaceProgressSnapshot& s) {
    p >> s.playerId;
    p >> s.nextCheckpoint;
    p >> s.finished;
    p >> s.disqualified;
    p >> s.elapsedTime;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const PlayerSnapshot& s) {
    p << s.id;
    p << s.name;
    p << s.car;
    p << s.raceProgress;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, PlayerSnapshot& s) {
    p >> s.id;
    p >> s.name;
    p >> s.car;
    p >> s.raceProgress;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const SpawnPointInfo& s) {
    p << s.id;
    p << s.x << s.y;
    p << s.angle;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, SpawnPointInfo& s) {
    p >> s.id;
    p >> s.x >> s.y;
    p >> s.angle;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CheckpointInfo& s) {
    p << s.id;
    p << s.order;
    p << s.x << s.y;
    p << s.w << s.h;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CheckpointInfo& s) {
    p >> s.id;
    p >> s.order;
    p >> s.x >> s.y;
    p >> s.w >> s.h;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const StaticSnapshot& s) {
    p << s.race;
    p << s.checkpoints;
    p << s.spawns;
    p << s.players;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, StaticSnapshot& s) {
    p >> s.race;
    p >> s.checkpoints;
    p >> s.spawns;
    p >> s.players;
    return p;
}
