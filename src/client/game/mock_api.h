#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include "common/structs.h"

enum class TurnDirection { Left, Right, None };

// TEMPORAL
struct RaceProgressSnapshot {
    PlayerId playerId;
    int nextCheckpoint;  // número de checkpoint pendiente
    bool finished;
    bool disqualified;
    float elapsedTime;
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
    // RaceProgressSnapshot raceProgress;
};
struct WorldSnapshot {
    float time{0.0f};  // tiempo global simulado
    std::string raceCity{"liberty"};
    std::string raceMapFile{"liberty"};

    // estado de MatchSession
    // MatchState matchState{MatchState::Starting};
    // uint32_t currentRaceIndex{0};

    // estado de RaceSession
    // RaceState raceState{RaceState::Countdown};
    // float raceElapsed{0.0f};
    // float raceCountdown{0.0f};

    float raceTimeLeft{0.0f};  // tiempo restante si hay límite (10min)
    std::vector<PlayerSnapshot> players;
    // std::vector<CollisionEvent> collisions;
    // std::vector<PlayerId> permanentlyDQ;
};

class MockApi {
    TurnDirection turn_direction = TurnDirection::None;
    bool accelerating = false;
    bool breaking = false;

    using clock = std::chrono::steady_clock;
    clock::time_point nitro_expired_at;

    WorldSnapshot snapshot = {.players{
        {.id = 0,
         .name{"P1"},
         .car = {.type = CarSpriteType::Speedster,
                 .x = 0,
                 .y = 0,
                 .vx = 0,
                 .vy = 0,
                 .angle = 0,
                 .speed = 0,
                 .health = 100,
                 .nitroActive = false,
                 .braking = false,
                 .accelerating = false}},
        {.id = 1,
         .name{"P2"},
         .car = {.type = CarSpriteType::Rocket,
                 .x = 100,
                 .y = 100,
                 .vx = 0,
                 .vy = 0,
                 .angle = 0,
                 .speed = 0,
                 .health = 100,
                 .nitroActive = false,
                 .braking = false,
                 .accelerating = false}},
    }};

   public:
    void start_turning(TurnDirection);
    void stop_turning(TurnDirection);
    void start_accelerating();
    void stop_accelerating();
    void start_breaking();
    void stop_breaking();
    void start_using_nitro();
    WorldSnapshot get_snapshot();

   private:
    void control_car(CarSnapshot&) const;
    bool controllable_has_nitro() const;
    static bool has_velocity(const CarSnapshot&);
};
