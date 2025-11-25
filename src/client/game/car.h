#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>

#include "client/game/game.h"
#include "common/macros.h"

class Game;  // friend class Car;

class Car final {
   private:
    Game& game;
    SDL2pp::Texture& sprite;

    // AUXILIAR: Conseguir el volumen de un sonido de este car, relativo a la
    // pantalla.
    int get_vol() const;

   public:
    Car(Game& game, const PlayerSnapshot& base);

    // Parametros
    const int id;
    const std::string name;
    const float x;
    const float y;
    const float angle;
    const float speed;
    const float health;
    const uint32_t next_checkpoint;

    // Constantes utiles
    const int SPRITE_WIDTH;
    const int SPRITE_HEIGHT;

    void set_camera();
    void sound_crash();
    void draw(bool with_name);

    // Calcula el angulo desde su centro al centro del siguiente checkpoint
    // Setea has_angle relativo a si el angulo existe
    float get_angle_to_next_checkpoint(bool& has_angle) const;

    MAKE_FIXED(Car)
};
