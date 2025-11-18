#include "client/game/car.h"

Car::Car(Game& game, const PlayerSnapshot& base)
    : game(game),
      id(base.id),
      name(base.name),
      x(base.car.x),
      y(base.car.y),
      angle(base.car.angle),
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

int Car::get_vol(double x, double y) {
    double dx = game.cam_world_x - x;
    double dy = game.cam_world_y - y;

    double dist = sqrt(dx * dx + dy * dy);

    int v = MIX_MAX_VOLUME - dist / 4;
    return (v >= 0) ? v : 0;
}

void Car::sound_crash() {
    // Atenuar el sonido basado en que tan lejos esta de la camara.
    game.mixer.PlayChannel(id, game.assets.sound_crash);
    game.mixer.SetVolume(id, get_vol(x, y));
}

size_t Car::get_id() { return id; }
float Car::get_health() { return health; }
float Car::get_speed() { return speed; }
