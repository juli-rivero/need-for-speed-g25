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
    int id;
    float x;
    float y;
    float angle;
    float speed;
    SDL2pp::Texture& sprite;

   public:
    Car(Game& game, const PlayerSnapshot& base);

    void set_camera();
    void sound_crash();
    void draw();

    size_t get_id();

    MAKE_FIXED(Car)
};
