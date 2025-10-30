#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "common/macros.h"

class Assets final {
   public:
    explicit Assets(SDL2pp::Renderer& renderer);

    SDL2pp::Texture car1;
    SDL2pp::Texture city_liberty;

    MAKE_FIXED(Assets)
};
