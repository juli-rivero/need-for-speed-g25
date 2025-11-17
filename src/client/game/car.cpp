#include "client/game/car.h"

#define MAX_SPEED 5

Car::Car(Game& game, const PlayerSnapshot& base)
    : game(game),
      id(base.id),
      x(base.car.x),
      y(base.car.y),
      angle(base.car.angle),
      speed(base.car.speed),
      sprite(*game.assets.car_name.at(base.car.type)) {}

void Car::set_camera() {
    game.cam_x = x + sprite.GetWidth() / 2 - game.renderer.GetOutputWidth() / 2;
    game.cam_y =
        y + sprite.GetHeight() / 2 - game.renderer.GetOutputHeight() / 2;
    game.cam_world_x = x;
    game.cam_world_y = y;
}

static inline int vol(double x1, double y1, double x2, double y2) {
    double dist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    int v = MIX_MAX_VOLUME - dist / 3;  // Quizas 4?
    return (v >= 0) ? v : 0;
}

void Car::draw() {
    game.render(sprite, x, y, angle);
    game.render(std::to_string(speed), x, y - 32, true);
}

void Car::sound_crash() {
    // Atenuar el sonido basado en que tan lejos esta de la camara.
    if (game.frame % 120 != 0) return;

    game.mixer.PlayChannel(id, game.assets.sound_crash);
    game.mixer.SetVolume(id, vol(x, y, game.cam_world_x, game.cam_world_y));
}

size_t Car::get_id() { return id; }
