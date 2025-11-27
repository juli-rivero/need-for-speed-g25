#include <math.h>

#include "client/game/classes.h"

constexpr float PI = 3.14;
constexpr float pixels_per_meter = 10;

static float world_to_pixel(const float meters) {
    return meters * pixels_per_meter;
}
static float radians_to_sdl_degrees(const float radians) {
    float degrees = radians * 180 / PI;
    degrees += 90;
    if (degrees > 360) degrees -= 360;
    if (degrees < 0) degrees += 360;
    return degrees;
}

Car::Car(Game& game, const PlayerSnapshot& base)
    : game(game),
      sprite(*game.assets.car_name.at(base.car.type)),
      id(base.id),
      name(base.name),
      x(world_to_pixel(base.car.bound.pos.x)),
      y(world_to_pixel(base.car.bound.pos.y)),
      angle(radians_to_sdl_degrees(base.car.angle)),
      speed(base.car.speed),
      health(base.car.health),
      next_checkpoint(base.raceProgress.nextCheckpoint),
      finished(base.raceProgress.finished),
      elapsed_time(base.raceProgress.elapsedTime),
      braking(base.car.braking),
      WIDTH(sprite.GetWidth()),
      HEIGHT(sprite.GetHeight()) {}

void Car::set_camera() {
    game.cam_x = x + WIDTH / 2 - game.screen.WIDTH / 2;
    game.cam_y = y + HEIGHT / 2 - game.screen.HEIGHT / 2;
    game.cam_world_x = x;
    game.cam_world_y = y;
}

void Car::draw(bool with_name) {
    game.screen.render(sprite, x, y, angle);
    if (with_name) game.screen.render(name, x, y - HEIGHT - 10, true);
}

float Car::get_angle_to_next_checkpoint(bool& has_angle) const {
    if (next_checkpoint >= game.race_info.checkpoints.size()) {
        has_angle = false;
        return 0;
    }

    float my_x = x + WIDTH / 2;
    float my_y = y + HEIGHT / 2;

    const CheckpointInfo& c = game.race_info.checkpoints[next_checkpoint];
    const Bound& cb = c.bound;
    float check_x =
        cb.pos.x * pixels_per_meter + cb.width * pixels_per_meter / 2;
    float check_y =
        cb.pos.y * pixels_per_meter + cb.height * pixels_per_meter / 2;

    has_angle = true;
    return atan2(my_y - check_y, my_x - check_x) * 180 / PI;
}
