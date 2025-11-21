#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum class SessionStatus { Waiting, Playing, Full };
enum class RenderLayer { UNDER = 0, OVER = 1 };

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

    float height;
    float width;

    float maxSpeed;
    float acceleration;
    float mass;
    float control;
    mutable float health;
};

// Game

enum class TurnDirection { None, Left, Right };

struct CollisionSimple {
    PlayerId player;
    float intensity;
};
struct CollisionCarToCar : CollisionSimple {
    PlayerId other;
    explicit CollisionCarToCar(const PlayerId player, const PlayerId other,
                               const float intensity)
        : CollisionSimple(player, intensity), other(other) {}
};

using CollisionEvent = std::variant<CollisionSimple, CollisionCarToCar>;
struct CollisionPacket {
    std::vector<CollisionEvent> events;
};

enum class MatchState { Starting, Racing, Intermission, Finished };

enum class RaceState { Countdown, Running, Finished };

struct CarSnapshot {
    CarSpriteType type;
    RenderLayer layer;
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

struct WorldSnapshot {
    float time{0.0f};  // tiempo global simulado
    std::string raceCity;
    std::string raceMapFile;

    // estado de MatchSession
    MatchState matchState{MatchState::Starting};
    uint32_t currentRaceIndex{0};

    // estado de RaceSession
    RaceState raceState{RaceState::Countdown};
    float raceElapsed{0.0f};
    float raceCountdown{0.0f};

    float raceTimeLeft{0.0f};  // tiempo restante si hay límite (10min)
    std::vector<PlayerSnapshot> players;
    std::vector<PlayerId> permanentlyDQ;
};
