#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>

#include "client/game/game.h"
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
    uint32_t next_checkpoint;
    SDL2pp::Texture& sprite;

    int get_vol() const;

   public:
    Car(Game& game, const PlayerSnapshot& base);

    void set_camera();
    void sound_crash();
    void draw(bool with_name);

    size_t get_id() const;
    float get_health() const;
    float get_speed() const;
    uint32_t get_next_checkpoint() const;

    MAKE_FIXED(Car)
};
