#include "common/dto/structs_serializable.h"

#include <variant>

#include "common/protocol.h"
#include "common/structs.h"

ProtocolReceiver& operator>>(ProtocolReceiver& p, StaticSessionData& s) {
    return p >> s.playersCapacity >> s.racesCapacity >> s.cities;
}
ProtocolSender& operator<<(ProtocolSender& p, const StaticSessionData& s) {
    return p << s.playersCapacity << s.racesCapacity << s.cities;
}

ProtocolSender& operator<<(ProtocolSender& p, const SessionConfig& s) {
    return p << s.name << s.maxPlayers << s.raceCount << s.city;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, SessionConfig& s) {
    return p >> s.name >> s.maxPlayers >> s.raceCount >> s.city;
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
ProtocolSender& operator<<(ProtocolSender& p, const CarDisplayInfo& s) {
    return p << s.name << s.description;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CarDisplayInfo& s) {
    return p >> s.name >> s.description;
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

ProtocolSender& operator<<(ProtocolSender& p, const CarStaticStats& s) {
    p << s.maxSpeed << s.acceleration << s.mass << s.control << s.maxHealth;
    p << s.nitroMultiplier << s.nitroDuration << s.nitroCooldown;
    p << s.width << s.height;
    p << s.density << s.friction << s.restitution << s.linearDamping
      << s.angularDamping << s.driftFactor;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CarStaticStats& s) {
    p >> s.maxSpeed >> s.acceleration >> s.mass >> s.control >> s.maxHealth;
    p >> s.nitroMultiplier >> s.nitroDuration >> s.nitroCooldown;
    p >> s.width >> s.height;
    p >> s.density >> s.friction >> s.restitution >> s.linearDamping >>
        s.angularDamping >> s.driftFactor;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const CarInfo& s) {
    return p << s.type << s.display << s.stats;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CarInfo& s) {
    return p >> s.type >> s.display >> s.stats;
}

ProtocolSender& operator<<(ProtocolSender& p, const Point& pos) {
    return p << pos.x << pos.y;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, Point& pos) {
    return p >> pos.x >> pos.y;
}
ProtocolSender& operator<<(ProtocolSender& p, const Bound& bound) {
    return p << bound.pos << bound.width << bound.height;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, Bound& bound) {
    return p >> bound.pos >> bound.width >> bound.height;
}

ProtocolSender& operator<<(ProtocolSender& p, const SpawnPointInfo& s) {
    return p << s.pos << s.angle;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, SpawnPointInfo& s) {
    return p >> s.pos >> s.angle;
}

ProtocolSender& operator<<(ProtocolSender& p, const CheckpointInfo& s) {
    return p << s.order << s.bound << s.angle << s.type;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, CheckpointInfo& s) {
    return p >> s.order >> s.bound >> s.angle >> s.type;
}

ProtocolSender& operator<<(ProtocolSender& p, const RaceInfo& race) {
    return p << race.name << race.checkpoints << race.spawnPoints;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, RaceInfo& race) {
    return p >> race.name >> race.checkpoints >> race.spawnPoints;
}
ProtocolSender& operator<<(ProtocolSender& p, const CityInfo& city) {
    return p << city.name << city.walls << city.bridges << city.railings
             << city.overpasses << city.lower_sensors << city.upper_sensors;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, CityInfo& city) {
    return p >> city.name >> city.walls >> city.bridges >> city.railings >>
           city.overpasses >> city.lower_sensors >> city.upper_sensors;
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
    p << s.layer;
    p << s.bound;
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
    p >> s.layer;
    p >> s.bound;
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
    p << s.nextCheckpoint;
    p << s.finished;
    p << s.disqualified;
    p << s.elapsed;
    p << s.penaltyTime;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, RaceProgressSnapshot& s) {
    p >> s.nextCheckpoint;
    p >> s.finished;
    p >> s.disqualified;
    p >> s.elapsed;
    p >> s.penaltyTime;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const UpgradeStats& s) {
    p << s.bonusAcceleration;
    p << s.bonusHealth;
    p << s.bonusMaxSpeed;
    p << s.bonusNitro;
    return p;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, UpgradeStats& s) {
    p >> s.bonusAcceleration;
    p >> s.bonusHealth;
    p >> s.bonusMaxSpeed;
    p >> s.bonusNitro;
    return p;
}

ProtocolSender& operator<<(ProtocolSender& p, const PlayerSnapshot& s) {
    p << s.id;
    p << s.name;
    p << s.car;
    p << s.raceProgress;
    p << s.upgrades;
    p << s.rawTime << s.penalty << s.netTime;
    return p;
}

ProtocolReceiver& operator>>(ProtocolReceiver& p, PlayerSnapshot& s) {
    p >> s.id;
    p >> s.name;
    p >> s.car;
    p >> s.raceProgress;
    p >> s.upgrades;
    p >> s.rawTime >> s.penalty >> s.netTime;
    return p;
}
ProtocolSender& operator<<(ProtocolSender& p, const NpcSnapshot& s) {
    return p << s.type << s.layer << s.bound << s.angle;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, NpcSnapshot& s) {
    return p >> s.type >> s.layer >> s.bound >> s.angle;
}
ProtocolSender& operator<<(ProtocolSender& p, const MatchSnapshot& m) {
    return p << m.matchState << m.currentRaceIndex << m.time << m.positions;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, MatchSnapshot& m) {
    return p >> m.matchState >> m.currentRaceIndex >> m.time >> m.positions;
}
ProtocolSender& operator<<(ProtocolSender& p, const RaceSnapshot& r) {
    return p << r.raceState << r.raceElapsed << r.raceCountdown
             << r.raceTimeLeft << r.positions;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, RaceSnapshot& r) {
    return p >> r.raceState >> r.raceElapsed >> r.raceCountdown >>
           r.raceTimeLeft >> r.positions;
}

ProtocolSender& operator<<(ProtocolSender& p, const GameSnapshot& s) {
    return p << s.match << s.race << s.players << s.npcs;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, GameSnapshot& s) {
    return p >> s.match >> s.race >> s.players >> s.npcs;
}

ProtocolSender& operator<<(ProtocolSender& p, const UpgradeChoice& s) {
    return p << s.stat << s.delta << s.penalty;
}
ProtocolReceiver& operator>>(ProtocolReceiver& p, UpgradeChoice& s) {
    return p >> s.stat >> s.delta >> s.penalty;
}
