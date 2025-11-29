#pragma once

#include <SDL2pp/SDL2pp.hh>
#include <string>
#include <unordered_map>

#include "common/macros.h"
#include "common/structs.h"

class Assets final {
   public:
    explicit Assets(SDL2pp::Renderer& renderer);

    // Assets a secas
    SDL2pp::Texture car_speedster;
    SDL2pp::Texture car_tank;
    SDL2pp::Texture car_drifter;
    SDL2pp::Texture car_rocket;
    SDL2pp::Texture car_classic;
    SDL2pp::Texture car_offroad;
    SDL2pp::Texture car_ghost;

    SDL2pp::Texture city_liberty;

    SDL2pp::Chunk sound_brake;
    SDL2pp::Chunk sound_crash;
    SDL2pp::Chunk sound_finish;

    SDL2pp::Font font;

    SDL2pp::Texture arrow;
    SDL2pp::Texture white;

    // Mapeos de cadena
    std::unordered_map<CarType, SDL2pp::Texture*> car_name;
    std::unordered_map<std::string, SDL2pp::Texture*> city_name;

    MAKE_FIXED(Assets)
};
