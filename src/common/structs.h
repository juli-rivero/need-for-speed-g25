#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum class SessionStatus { Waiting, Playing, Full };

using CityName = std::string;
using RaceName = std::string;

#define SESSION_CONFIG_FIELDS \
    std::string name;         \
    uint8_t maxPlayers;       \
    uint8_t raceCount;        \
    CityName city;

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

enum class CarType {
    Speedster,
    Tank,
    Drifter,
    Rocket,
    Classic,
    Offroad,
    Ghost
};

struct CarDisplayInfo {
    std::string name;
    std::string description;
};

// Estructura para representar un jugador en la sala de espera
struct PlayerInfo {
    PlayerId id;
    std::string name;
    CarType carType;
    bool isReady;
    bool isHost;
};

struct CarStaticStats {
    // --- atributos de rendimiento ---
    float maxSpeed;
    float acceleration;
    float mass;
    float control;
    float maxHealth;

    // --- nitro ---
    float nitroMultiplier;
    float nitroDuration;
    float nitroCooldown;

    // --- físicas ---
    float width;
    float height;

    float density;
    float friction;
    float restitution;
    float linearDamping;
    float angularDamping;
};

struct CarInfo {
    CarType type;
    CarDisplayInfo display;
    CarStaticStats stats;
};

// Game

struct Point {
    float x, y;
};

struct Bound {
    Point pos;
    float width, height;
};

enum class TurnDirection { None, Left, Right };

struct CollisionSimple {
    PlayerId player;
    float intensity;
};

struct CollisionCarToCar : CollisionSimple {
    PlayerId other;

    CollisionCarToCar(PlayerId player, PlayerId other, float intensity)
        : CollisionSimple(player, intensity), other(other) {}
};

using CollisionEvent = std::variant<CollisionSimple, CollisionCarToCar>;

enum class RenderLayer { UNDER = 0, OVER = 1 };

struct CarSnapshot {
    CarType type;
    RenderLayer layer;
    Bound bound;
    float vx, vy;       // velocidad lineal
    float angle;        // orientación (radianes)
    float speed;        // módulo de la velocidad
    float health;       // salud del vehículo
    bool nitroActive;   // nitro encendido
    bool braking;       // si está frenando
    bool accelerating;  // si está acelerando
};

struct RaceProgressSnapshot {
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
    Point pos;
    float angle;
};

enum class CheckpointType { Start, Intermediate, Finish };

struct CheckpointInfo {
    uint32_t order;
    Bound bound;
    float angle;
    CheckpointType type;
};
struct NpcInfo {
    float x, y, angle, w, h;
    CarType type;
};
struct RaceInfo {
    RaceName name;
    std::vector<CheckpointInfo> checkpoints;
    std::vector<SpawnPointInfo> spawnPoints;
};

struct CityInfo {
    CityName name;
    std::vector<Bound> walls;
    std::vector<Bound> bridges;
    std::vector<Bound> railings;
    std::vector<Bound> overpasses;
};

enum class MatchState { Starting, Racing, Intermission, Finished };

enum class RaceState { Countdown, Running, Finished };
