
#ifndef TALLER_TP_NETWORKTYPES_H
#define TALLER_TP_NETWORKTYPES_H
#include <string>
#include <vector>
#include <unordered_map>

// ========= INPUTS ==========
struct PlayerInput {
    bool accelerate{false};
    bool brake{false};
    bool nitro{false};
    std::string turn;  // "left", "right", "none"
};

// ========= SNAPSHOTS ==========

// estado individual del auto
struct CarSnapshot {
    int id;            // playerId
    float x, y;        // posición actual
    float vx, vy;      // velocidad lineal
    float angle;       // orientación (radianes)
    float speed;       // módulo de la velocidad
    float health;      // salud del vehículo
    bool nitroActive;  // nitro encendido
    bool braking;      // si está frenando
    bool accelerating; // si está acelerando
};

struct RaceProgressSnapshot {
    int playerId;
    int nextCheckpoint;   // número de checkpoint pendiente
    bool finished;
    bool disqualified;
    float elapsedTime;
};

struct WorldSnapshot {
    float time;            // tiempo global simulado
    float raceTimeLeft;    // tiempo restante si hay límite
    std::vector<CarSnapshot> cars;
    std::vector<RaceProgressSnapshot> raceProgress;
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
    int id;
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

// snapshot estático (solo al empezar la partida)
struct StaticSnapshot {
    std::string mapName;
    std::string cityName;

    std::vector<WallInfo> walls;
    std::vector<CheckpointInfo> checkpoints;
    std::vector<HintInfo> hints;
    std::vector<SpawnPointInfo> spawns;
    std::vector<CarStaticInfo> cars;
};
#endif //TALLER_TP_NETWORKTYPES_H