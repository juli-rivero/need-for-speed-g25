#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include "common/structs.h"

class MockApi {
    TurnDirection turn_direction = TurnDirection::None;
    bool accelerating = false;
    bool breaking = false;

    using clock = std::chrono::steady_clock;
    clock::time_point nitro_expired_at;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

    WorldSnapshot snapshot = {.players{
        {
            .id = 0,
            .name = {"P1"},
            .car = {.type = CarSpriteType::Speedster,
                    .x = 0,
                    .y = 0,
                    .angle = 0,
                    .speed = 0,
                    .health = 100,
                    .nitroActive = false,
                    .braking = false,
                    .accelerating = false},
        },
        {
            .id = 1,
            .name{"P2"},
            .car = {.type = CarSpriteType::Rocket,
                    .x = 100,
                    .y = 100,
                    .angle = 0,
                    .speed = 0,
                    .health = 100,
                    .nitroActive = false,
                    .braking = false,
                    .accelerating = false},
        },
    }};

#pragma GCC diagnostic pop

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
