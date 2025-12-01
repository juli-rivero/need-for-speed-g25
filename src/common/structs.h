#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum class SessionStatus { Waiting, Playing, Full };

using CityName = std::string;
using RaceName = std::string;

struct StaticSessionData {
    uint32_t playersCapacity;
    uint32_t racesCapacity;
    std::vector<CityName> cities;
};

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
    float driftFactor;

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

// Pre-Game (al empezar la partida)

struct Point {
    float x, y;
};

struct Bound {
    Point pos;
    float width, height;
};

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

// Game

enum class TurnDirection { None, Left, Right };

struct CollisionSimple {
    PlayerId player;
    float intensity;
};

struct CollisionCarToCar : CollisionSimple {
    PlayerId other;

    CollisionCarToCar(PlayerId player, PlayerId other, float intensity)
        : CollisionCarToCar::CollisionSimple{player, intensity}, other(other) {}
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

struct UpgradeStats {
    float bonusMaxSpeed = 0;
    float bonusAcceleration = 0;
    float bonusHealth = 0;
    float bonusNitro = 0;
};

struct PlayerSnapshot {
    PlayerId id;       // ID del jugador
    std::string name;  // nombre del jugador
    CarSnapshot car;   // posición, velocidad, etc.
    RaceProgressSnapshot raceProgress;
    UpgradeStats upgrades;
    float rawTime;  // tiempo crudo final de TODAS las carreras
    float penalty;  // penalidad total acumulada
    float netTime;  // rawTime + penalty
};
struct NpcSnapshot {
    CarType type;
    RenderLayer layer;
    Bound bound;
    float angle;
};

enum class RaceState { Countdown, Running, Finished };
struct RaceSnapshot {
    RaceState raceState{RaceState::Countdown};
    float raceElapsed{0.0f};    // tiempo desde que empezo la carrera
    float raceCountdown{0.0f};  // tiempo restante antes de empezar la racha
    float raceTimeLeft{0.0f};   // tiempo restante si hay límite (10min)
    std::vector<PlayerId> positions;
};

enum class MatchState { Starting, Racing, Intermission, Finished };
struct MatchSnapshot {
    MatchState matchState{MatchState::Starting};
    uint32_t currentRaceIndex{0};
    float time{0.0f};  // tiempo global simulado
    std::vector<PlayerId> positions;
};
struct GameSnapshot {
    std::vector<PlayerSnapshot> players;
    std::vector<NpcSnapshot> npcs;

    MatchSnapshot match;
    RaceSnapshot race;
};

enum class Cheat {
    FinishRace,
    DestroyAllCars,
    InfiniteHealth,
};

enum class UpgradeStat { Acceleration, MaxSpeed, Nitro, Health };
struct UpgradeChoice {
    UpgradeStat stat;
    float delta;    // cuánto aumenta el stat
    float penalty;  // penalidad por aplicar este upgrade
};
