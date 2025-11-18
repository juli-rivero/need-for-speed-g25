#include "client/game/car.h"

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
      id(base.id),
      name(base.name),
      x(world_to_pixel(base.car.x)),
      y(world_to_pixel(base.car.y)),
      angle(radians_to_sdl_degrees(base.car.angle)),
      speed(base.car.speed),
      health(base.car.health),
      sprite(*game.assets.car_name.at(base.car.type)) {}

void Car::set_camera() {
    game.cam_x = x + sprite.GetWidth() / 2 - game.renderer.GetOutputWidth() / 2;
    game.cam_y =
        y + sprite.GetHeight() / 2 - game.renderer.GetOutputHeight() / 2;
    game.cam_world_x = x;
    game.cam_world_y = y;
}

void Car::draw(bool with_name) {
    game.render(sprite, x, y, angle);
    if (with_name) game.render(name, x, y - 32, true);
}

int Car::get_vol() const {
    const double dx = game.cam_world_x - x;
    const double dy = game.cam_world_y - y;

    const double dist = sqrt(dx * dx + dy * dy);

    const int v = MIX_MAX_VOLUME - dist / 4;
    return (v >= 0) ? v : 0;
}

void Car::sound_crash() {
    // Atenuar el sonido basado en que tan lejos esta de la camara.
    if (game.frame % 120 != 0) return;

    game.mixer.PlayChannel(id, game.assets.sound_crash);
    game.mixer.SetVolume(id, get_vol());
}

size_t Car::get_id() const { return id; }
float Car::get_health() const { return health; }
float Car::get_speed() const { return speed; }
