#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>

#include "client/game/game.h"
#include "client/game/mock_api.h"
#include "common/macros.h"

class Game;  // friend class Car;

class Car final {
   private:
    Game& game;

    // Parametros mutables
    int id;
    std::string name;
    float x;
    float y;
    float angle;
    float speed;
    float health;
    SDL2pp::Texture& sprite;

    int get_vol(double x, double y);

   public:
    Car(Game& game, const PlayerSnapshot& base);

    void set_camera();
    void sound_crash();
    void draw(bool with_name);

    size_t get_id();
    float get_health();
    float get_speed();

    MAKE_FIXED(Car)
};
