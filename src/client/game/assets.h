#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "common/macros.h"

class Assets final {
   public:
    explicit Assets(SDL2pp::Renderer& renderer);

    SDL2pp::Texture car1;
    SDL2pp::Texture car2;
    SDL2pp::Texture car3;
    SDL2pp::Texture car4;
    SDL2pp::Texture car5;
    SDL2pp::Texture car6;
    SDL2pp::Texture car7;

    SDL2pp::Texture city_liberty;

    SDL2pp::Chunk sound_brake;
    SDL2pp::Chunk sound_crash;
    SDL2pp::Chunk sound_finish;

    SDL2pp::Font font;

    MAKE_FIXED(Assets)
};
