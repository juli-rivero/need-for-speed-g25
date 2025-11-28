#include <math.h>

#include "client/game/classes.h"
#include "spdlog/spdlog.h"

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
      layer(base.car.layer),
      WIDTH(sprite.GetWidth()),
      HEIGHT(sprite.GetHeight()) {}

void Car::set_camera() {
    game.cam_x = x + WIDTH / 2 - game.screen.WIDTH / 2;
    game.cam_y = y + HEIGHT / 2 - game.screen.HEIGHT / 2;
    game.cam_world_x = x;
    game.cam_world_y = y;
}

void Car::draw(bool with_name) {
    int x_i = static_cast<int>(x);
    int y_i = static_cast<int>(y);

    game.screen.render(sprite, {x_i, y_i}, angle);
    if (with_name)
        game.screen.render_text(name, {x_i, y_i - HEIGHT - 10}, true);
}

float Car::get_angle_to_next_checkpoint(bool& has_angle) const {
    if (next_checkpoint >= game.map.checkpoint_amount) {
        has_angle = false;
        return 0;
    }

    float my_x = x + WIDTH / 2;
    float my_y = y + HEIGHT / 2;

    const Map::Box& b = game.map.checkpoints[next_checkpoint];
    float check_x = b.x + b.w / 2;
    float check_y = b.y + b.h / 2;

    has_angle = true;
    return atan2(my_y - check_y, my_x - check_x) * 180 / PI;
}
