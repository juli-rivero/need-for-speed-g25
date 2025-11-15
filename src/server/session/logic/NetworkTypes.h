#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "server/session/logic/types.h"

// ========= INPUTS ==========
enum class TurnDirection { None, Left, Right };

struct PlayerInput {
    bool accelerate{false};
    bool brake{false};
    bool nitro{false};
    bool leftPressed{false};
    bool rightPressed{false};
    TurnDirection turn{TurnDirection::None};
};
// ========= Events ==========
enum class CollisionType { CarToCar, CarToWall };
static constexpr PlayerId INVALID_PLAYER = UINT32_MAX;
struct CollisionEvent {
    CollisionType type;
    PlayerId carA;
    PlayerId carB;
    float intensity;
};

// ========= SNAPSHOTS ==========

struct RaceProgressSnapshot {
    PlayerId playerId;
    int nextCheckpoint;  // número de checkpoint pendiente
    bool finished;
    bool disqualified;
    float elapsedTime;
};

enum class CarSpriteType {
    Speedster,
    Muscle,
    Offroad,
    Truck
    // y asi..
};
// estado individual del auto
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
struct PlayerSnapshot {
    PlayerId id;       // ID del jugador
    std::string name;  // nombre del jugador
    CarSnapshot car;   // posición, velocidad, etc.
    RaceProgressSnapshot raceProgress;
};
struct WorldSnapshot {
    float time{0.0f};          // tiempo global simulado
    float raceTimeLeft{0.0f};  // tiempo restante si hay límite (10min)
    std::vector<PlayerSnapshot> players;
    std::vector<CollisionEvent> collisions;
};
struct WallInfo {
    int id;
    float x, y;
    float w, h;
};

struct CheckpointInfo {
    int id;
    int order;
    float x, y;
    float w;
    float h;
};

struct HintInfo {
    int id;
    float x, y;
};

struct SpawnPointInfo {
    int id;
    float x, y;
    float angle;
};

struct CarStaticInfo {
    PlayerId id;  // seria el id del jugador que tiene dicho auto
    std::string playerName;
    std::string carType;
    float width;
    float height;
    float maxSpeed;
    float acceleration;
    float control;
    float friction;
    float nitroMultiplier;
};
struct BridgeInfo {
    int id;
    float lowerX, lowerY;
    float upperX, upperY;
    float w, h;
    bool driveable;
};

// snapshot estático (solo al empezar la partida)
struct StaticSnapshot {
    std::string mapName;
    std::string cityName;

    std::vector<WallInfo> walls;
    std::vector<BridgeInfo> bridges;
    std::vector<CheckpointInfo> checkpoints;
    std::vector<HintInfo> hints;
    std::vector<SpawnPointInfo> spawns;
    std::vector<CarStaticInfo> cars;
};
