#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "client/game/game.h"
#include "common/macros.h"

class Game;  // friend class Car;

class Car final {
   private:
    Game& game;

    // Parametros mutables
    double x;
    double y;
    double speed;
    double angle;
    SDL2pp::Texture& sprite;
    size_t id;

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

   public:
    Car(Game& game, double x, double y, double speed, double angle,
        SDL2pp::Texture& sprite, size_t id);

    void update();  // TEMP

    void set_camera();
    void sound_crash();
    void draw();

    size_t get_id();

    // MAKE_FIXED(Car)
};
