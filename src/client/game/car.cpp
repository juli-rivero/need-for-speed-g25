#include "client/game/car.h"

#define MAX_SPEED 5

Car::Car(Game& game, double x, double y, double speed, double angle,
         SDL2pp::Texture& sprite, bool first)
    : game(game),
      x(x),
      y(y),
      speed(speed),
      angle(angle),
      sprite(sprite),
      first(first),
      WIDTH(sprite.GetWidth()),
      HEIGHT(sprite.GetHeight()) {}

void Car::update() {
    if (first) {
        if (game.left_held) angle -= 3;
        if (game.right_held) angle += 3;
        if (game.down_held) speed -= 0.1;

        if (game.up_held) {
            speed += 0.1;
        } else {
            if (speed > 0) {
                speed -= 0.1;
            } else {
                speed += 0.1;
            }
        }
    } else {
        angle += 3;
    }

    if (angle > 360) angle -= 360;
    if (angle < 0) angle += 360;
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    if (speed < -MAX_SPEED) speed = -MAX_SPEED;

    x += sin(-angle * 3.14 / 180) * (-speed);
    y += cos(-angle * 3.14 / 180) * (-speed);
}

void Car::set_camera() {
    game.cam_x = x + WIDTH / 2 - game.renderer.GetOutputWidth() / 2;
    game.cam_y = y + HEIGHT / 2 - game.renderer.GetOutputHeight() / 2;
}

void Car::draw() {
    game.render(sprite, x, y, angle);
    game.render(std::to_string(speed), x, y - 32, true);
}

void Car::play(SDL2pp::Chunk& sound) { game.mixer.PlayChannel(-1, sound); }

bool Car::is_first() { return first; }
