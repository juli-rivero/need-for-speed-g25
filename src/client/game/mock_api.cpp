#include "client/game/mock_api.h"

#include <algorithm>
#include <cmath>

#include "spdlog/spdlog.h"

#define DELTA_TIME (1.0 / 60.0)  // s

#define ACCELERATION (250.0)        // m/s^2
#define BREAKING (250.0)            // m/s^2
#define REVERSING (100.0)           // m/s^2
#define NITRO_ACCELERATION (250.0)  // m/s^2

#define FRICTION_FACTOR (1)  // 1/s

#define TO_RADIANS(x) ((x) * 3.14 / 180.0)

void MockApi::start_turning(const TurnDirection aux) { turn_direction = aux; }
void MockApi::stop_turning(const TurnDirection direction_to_stop) {
    if (turn_direction == direction_to_stop)
        turn_direction = TurnDirection::None;
}
void MockApi::start_accelerating() {
    spdlog::trace("accelerating");
    accelerating = true;
}
void MockApi::stop_accelerating() {
    spdlog::trace("stop_accelerating");
    accelerating = false;
}
void MockApi::start_breaking() { breaking = true; }
void MockApi::stop_breaking() { breaking = false; }
void MockApi::start_using_nitro() {
    nitro_expired_at = clock::now() + std::chrono::seconds(10);
}

Snapshot MockApi::get_snapshot() {
    control_car(snapshot.cars[0]);

    for (auto& car : snapshot.cars) {
        // m   =            k                        *     m/s     *    s
        if (has_velocity(car)) {
            const auto aux_x = car.x;
            const auto aux_y = car.y;
            car.x +=
                -std::sin(TO_RADIANS(-car.angle)) * (car.speed * DELTA_TIME);
            car.y +=
                -std::cos(TO_RADIANS(-car.angle)) * (car.speed * DELTA_TIME);
            spdlog::trace("angle {}, speed {}, desplazamiento x {}, y {}",
                          car.angle, car.speed, car.x - aux_x, car.y - aux_y);
        }
    }

    return snapshot;
}

void MockApi::control_car(CarSnapshot& car) const {
    if (turn_direction == TurnDirection::Left) car.angle -= 3;
    if (turn_direction == TurnDirection::Right) car.angle += 3;
    //                        m/s    -=   m/s    *       1/s       *    s
    if (has_velocity(car))
        car.speed -= car.speed * FRICTION_FACTOR * DELTA_TIME;

    //               m/s    ±=   m/s^2  *   s
    if (breaking)
        car.speed -= (car.speed > 0 ? BREAKING : REVERSING) * DELTA_TIME;

    if (accelerating) car.speed += ACCELERATION * DELTA_TIME;

    car.nitro_active = false;
    if (controllable_has_nitro()) {
        car.nitro_active = true;
        car.speed += NITRO_ACCELERATION * DELTA_TIME;
    }

    if (car.angle > 360) car.angle -= 360;
    if (car.angle < 0) car.angle += 360;

    spdlog::trace("speed: {}", car.speed);
}
bool MockApi::controllable_has_nitro() const {
    return clock::now() < nitro_expired_at;
}
bool MockApi::has_velocity(const CarSnapshot& car) {
    return std::abs(car.speed) > 0.0001;
}
