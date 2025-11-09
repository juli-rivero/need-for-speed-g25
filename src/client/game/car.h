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

    bool first;

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

   public:
    Car(Game& game, double x, double y, double speed, double angle,
        SDL2pp::Texture& sprite, bool first);

    void update();  // TEMP

    void set_camera();
    void play(SDL2pp::Chunk& sound);
    void draw();

    bool is_first();

    // MAKE_FIXED(Car)
};
