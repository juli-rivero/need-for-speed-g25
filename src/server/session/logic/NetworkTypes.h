
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
    int id;
    std::string playerName;
    float x{0}, y{0};
    float angle{0};
    float vx{0}, vy{0};
    float speed{0};
    int checkpoint{0};
    int lap{1};
    bool nitro{false};
    float health{100.0f};
};


struct WorldSnapshot {
    float time{0};
    float raceTimeLeft{0};
    std::vector<CarSnapshot> cars;
};

// snapshot estático (solo al empezar la partida)
struct StaticSnapshot {
    std::string mapName;
    std::vector<std::pair<float,float>> walls;      // posiciones básicas
    std::vector<std::pair<float,float>> checkpoints;
    std::vector<std::pair<float,float>> hints;
    std::vector<std::pair<float,float>> spawnPoints;
};
#endif //TALLER_TP_NETWORKTYPES_H