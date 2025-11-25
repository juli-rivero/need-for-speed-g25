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
      x(world_to_pixel(base.car.x)),
      y(world_to_pixel(base.car.y)),
      angle(radians_to_sdl_degrees(base.car.angle)),
      speed(base.car.speed),
      health(base.car.health),
      next_checkpoint(base.raceProgress.nextCheckpoint),
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

int Car::get_vol() const {
    const double dx = game.cam_world_x - x;
    const double dy = game.cam_world_y - y;

    const double dist = std::sqrt(dx * dx + dy * dy);

    const int v = MIX_MAX_VOLUME - dist / 3;
    return (v >= 0) ? v : 0;
}

void Car::sound_crash() {
    // Atenuar el sonido basado en que tan lejos esta de la camara.
    game.mixer.PlayChannel(id, game.assets.sound_crash);
    game.mixer.SetVolume(id, get_vol());
}

float Car::get_angle_to_next_checkpoint(bool& has_angle) const {
    if (next_checkpoint >= game.setup.info.checkpoints.size()) {
        has_angle = false;
        return 0;
    }

    float my_x = x + WIDTH / 2;
    float my_y = y + HEIGHT / 2;

    const CheckpointInfo& c = game.setup.info.checkpoints[next_checkpoint];
    float check_x = c.x * pixels_per_meter + c.w * pixels_per_meter / 2;
    float check_y = c.y * pixels_per_meter + c.h * pixels_per_meter / 2;

    has_angle = true;
    return atan2(my_y - check_y, my_x - check_x) * 180 / PI;
}
