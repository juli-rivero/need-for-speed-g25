#pragma once
#include <chrono>
#include <optional>
#include <vector>

enum class TurnDirection { Left, Right, None };

struct CarSnapshot {
    int id;
    float x, y;  // m
    float angle;
    float speed;  // m/s
    bool nitro_active;
};

struct Snapshot {
    std::vector<CarSnapshot> cars;
};

class MockApi {
    TurnDirection turn_direction = TurnDirection::None;
    bool accelerating = false;
    bool breaking = false;

    using clock = std::chrono::steady_clock;
    clock::time_point nitro_expired_at;

    Snapshot snapshot = {.cars{
        {.id = 0,
         .x = 0,
         .y = 0,
         .angle = 0,
         .speed = 0,
         .nitro_active = false},
        {.id = 1,
         .x = 100,
         .y = 100,
         .angle = 3,
         .speed = 1,
         .nitro_active = false},
        {.id = 2,
         .x = 200,
         .y = 200,
         .angle = 3,
         .speed = 1,
         .nitro_active = false},
        {.id = 3,
         .x = 300,
         .y = 300,
         .angle = 3,
         .speed = 1,
         .nitro_active = false},
        {.id = 4,
         .x = 400,
         .y = 400,
         .angle = 3,
         .speed = 1,
         .nitro_active = false},
        {.id = 5,
         .x = 500,
         .y = 500,
         .angle = 3,
         .speed = 1,
         .nitro_active = false},
        {.id = 6,
         .x = 600,
         .y = 600,
         .angle = 3,
         .speed = 1,
         .nitro_active = false},
    }};

   public:
    void start_turning(TurnDirection);
    void stop_turning(TurnDirection);
    void start_accelerating();
    void stop_accelerating();
    void start_breaking();
    void stop_breaking();
    void start_using_nitro();
    Snapshot get_snapshot();

   private:
    void control_car(CarSnapshot&) const;
    bool controllable_has_nitro() const;
    static bool has_velocity(const CarSnapshot&);
};
