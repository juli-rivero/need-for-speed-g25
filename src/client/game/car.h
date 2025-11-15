#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "client/game/game.h"
#include "client/game/mock_api.h"
#include "common/macros.h"

class Game;  // friend class Car;

class Car final {
   private:
    Game& game;

    // Parametros mutables
    float x;
    float y;
    float angle;
    SDL2pp::Texture& sprite;
    size_t id;

    // Constantes utiles
    const int WIDTH;
    const int HEIGHT;

   public:
    Car(Game& game, SDL2pp::Texture& sprite, size_t id);

    void update(const CarSnapshot&);  // TEMP

    void set_camera();
    void sound_crash();
    void draw();

    size_t get_id();

    // MAKE_FIXED(Car)
};
