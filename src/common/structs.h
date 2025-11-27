#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum class SessionStatus { Waiting, Playing, Full };

#define SESSION_CONFIG_FIELDS \
    std::string name;         \
    uint8_t maxPlayers;       \
    uint8_t raceCount;        \
    std::string city;

// Estructura para crear una partida
struct SessionConfig {
    SESSION_CONFIG_FIELDS
};

// Estructura para representar una partida
struct SessionInfo {
    SESSION_CONFIG_FIELDS
    uint8_t currentPlayers;
    SessionStatus status;
};

using PlayerId = std::uint32_t;

enum class CarSpriteType {
    Speedster,
    Tank,
    Drifter,
    Rocket,
    Classic,
    Offroad,
    Ghost
};

// Estructura para representar un jugador en la sala de espera
struct PlayerInfo {
    PlayerId id;
    std::string name;
    CarSpriteType carType;
    bool isReady;
    bool isHost;
};

struct CarStaticInfo {
    CarSpriteType type;

    std::string name;
    std::string description;

    float maxSpeed;
    float acceleration;
    float mass;
    float control;
    float health;
};

// Game

enum class TurnDirection { None, Left, Right };

struct CollisionSimple {
    PlayerId player;
    float intensity;

    CollisionSimple() = default;

    CollisionSimple(PlayerId p, float i) : player(p), intensity(i) {}
};

struct CollisionCarToCar : CollisionSimple {
    PlayerId other;

    CollisionCarToCar(PlayerId player, PlayerId other, float intensity)
        : CollisionSimple(player, intensity), other(other) {}
};

using CollisionEvent = std::variant<CollisionSimple, CollisionCarToCar>;

enum class MatchState { Starting, Racing, Intermission, Finished };

enum class RaceState { Countdown, Running, Finished };

struct CarSnapshot {
    CarSpriteType type;
    float x, y;         // posición actual
    float vx, vy;       // velocidad lineal
    float angle;        // orientación (radianes)
    float speed;        // módulo de la velocidad
    float health;       // salud del vehículo
    bool nitroActive;   // nitro encendido
    bool braking;       // si está frenando
    bool accelerating;  // si está acelerando
};

struct RaceProgressSnapshot {
    PlayerId playerId;
    uint32_t nextCheckpoint;  // número de checkpoint pendiente
    bool finished;
    bool disqualified;
    float elapsedTime;
};

struct PlayerSnapshot {
    PlayerId id;       // ID del jugador
    std::string name;  // nombre del jugador
    CarSnapshot car;   // posición, velocidad, etc.
    RaceProgressSnapshot raceProgress;
};

// Pre-Game (al empezar la partida)

struct SpawnPointInfo {
    uint32_t id;
    float x, y;
    float angle;
};

struct CheckpointInfo {
    uint32_t id;
    uint32_t order;
    float x, y;
    float w;
    float h;
};

struct StaticSnapshot {
    std::string race;

    std::vector<CheckpointInfo> checkpoints;
    std::vector<SpawnPointInfo> spawns;
    std::vector<PlayerSnapshot> players;
};
